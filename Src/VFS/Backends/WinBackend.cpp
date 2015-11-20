#ifdef _WIN32
#include "WinBackend.h"

#include <Util/Utils.h>
#include <VFS/Files/NativeFile.h>

#include <Shlwapi.h>
#include <Shlobj.h>
#include <stack>

#pragma comment(lib, "Shlwapi.lib")

using namespace VFS;
using namespace VFS::Backends;

WinBackend::WinBackend(const std::string& p_BasePath, bool p_CanWrite, bool p_CanDelete, bool p_ForceCreate) : 
	m_BasePath(p_BasePath), 
	m_CanWrite(p_CanWrite), 
	m_CanDelete(p_CanDelete)
{
	m_BasePath = Util::Utils::StringTrimRight(m_BasePath, '/');
	m_BasePath = Util::Utils::StringTrimRight(m_BasePath, '\\');

	m_BasePath = Util::Utils::StringReplace(m_BasePath, "/", "\\");
	m_BasePath = Util::Utils::StringReplace(m_BasePath, "\\\\", "\\");

	m_BasePath += "\\";

	DWORD s_Attributes = GetFileAttributes(m_BasePath.c_str());

	// Base path doesn't exist
	if (!(s_Attributes != INVALID_FILE_ATTRIBUTES && (s_Attributes & FILE_ATTRIBUTE_DIRECTORY)))
	{
		if (p_ForceCreate && SHCreateDirectoryExA(NULL, m_BasePath.c_str(), NULL) == ERROR_SUCCESS)
			return;

		m_BasePath = "";
	}
}

WinBackend::~WinBackend()
{

}

bool WinBackend::FileExists(const std::string& p_Path)
{
	if (m_BasePath == "")
		return false;

	std::string s_Path = m_BasePath + p_Path;
	s_Path = Util::Utils::StringReplace(s_Path, "/", "\\");
	s_Path = Util::Utils::StringReplace(s_Path, "\\\\", "\\");

	char s_CanonicalPath[MAX_PATH];
	if (!PathCanonicalizeA(s_CanonicalPath, s_Path.c_str()))
		return false;

	if (!Util::Utils::StringStartWith(s_CanonicalPath + std::string("\\"), m_BasePath))
		return false;

	DWORD s_Attributes = GetFileAttributes(s_CanonicalPath);

	return (s_Attributes != INVALID_FILE_ATTRIBUTES && !(s_Attributes & FILE_ATTRIBUTE_DIRECTORY));
}

bool WinBackend::ReadFile(const std::string& p_Path, std::string& p_Data)
{
	if (m_BasePath == "")
		return false;

	std::string s_Path = m_BasePath + p_Path;
	s_Path = Util::Utils::StringReplace(s_Path, "/", "\\");
	s_Path = Util::Utils::StringReplace(s_Path, "\\\\", "\\");

	char s_CanonicalPath[MAX_PATH];
	if (!PathCanonicalizeA(s_CanonicalPath, s_Path.c_str()))
		return false;

	if (!Util::Utils::StringStartWith(s_CanonicalPath + std::string("\\"), m_BasePath))
		return false;

	DWORD s_Attributes = GetFileAttributes(s_CanonicalPath);

	// Check if file exists and is not a directory.
	if (!(s_Attributes != INVALID_FILE_ATTRIBUTES && !(s_Attributes & FILE_ATTRIBUTE_DIRECTORY)))
		return false;

	FILE* s_File = fopen(s_Path.c_str(), "rb");

	if (s_File == NULL)
		return false;

	// Find the size of the file.
#ifdef _WIN64
	_fseeki64(s_File, 0, SEEK_END);
	size_t s_Length = _ftelli64(s_File);
	_fseeki64(s_File, 0, SEEK_SET);
#else
	fseek(s_File, 0, SEEK_END);
	size_t s_Length = ftell(s_File);
	fseek(s_File, 0, SEEK_SET);
#endif

	// Read all the file data.
	char* s_Data = new char[s_Length];
	size_t s_ReadLength = fread(s_Data, 1, s_Length, s_File);

	p_Data = std::string(s_Data, s_ReadLength);
	free(s_Data);

	fclose(s_File);

	return true;
}

bool WinBackend::OpenFile(const std::string& p_Path, const std::string& p_Mode, FSFile** p_File)
{
	if (m_BasePath == "")
		return false;

	std::string s_Path = m_BasePath + p_Path;
	s_Path = Util::Utils::StringReplace(s_Path, "/", "\\");
	s_Path = Util::Utils::StringReplace(s_Path, "\\\\", "\\");

	char s_CanonicalPath[MAX_PATH];
	if (!PathCanonicalizeA(s_CanonicalPath, s_Path.c_str()))
		return false;

	if (!Util::Utils::StringStartWith(s_CanonicalPath + std::string("\\"), m_BasePath))
		return false;

	FILE* s_File = fopen(s_Path.c_str(), p_Mode.c_str());

	if (s_File == NULL)
		return false;

	*p_File = new VFS::Files::NativeFile(s_File);

	return true;
}

bool WinBackend::WriteFile(const std::string& p_Path, const std::string& p_Data, bool p_Append)
{
	if (m_BasePath == "" || !m_CanWrite)
		return false;

	std::string s_Path = m_BasePath + p_Path;
	s_Path = Util::Utils::StringReplace(s_Path, "/", "\\");
	s_Path = Util::Utils::StringReplace(s_Path, "\\\\", "\\");

	char s_CanonicalPath[MAX_PATH];
	if (!PathCanonicalizeA(s_CanonicalPath, s_Path.c_str()))
		return false;

	if (!Util::Utils::StringStartWith(s_CanonicalPath + std::string("\\"), m_BasePath))
		return false;

	std::string s_PathString = s_CanonicalPath;

	std::string s_Directory;
	const size_t s_LastIndex = s_PathString.rfind('\\');

	if (std::string::npos != s_LastIndex)
		s_Directory = s_PathString.substr(0, s_LastIndex);

	SHCreateDirectoryExA(NULL, s_Directory.c_str(), NULL);

	FILE* s_File = fopen(s_CanonicalPath, p_Append ? "ab" : "wb");

	if (s_File == NULL)
		return false;

	fwrite(p_Data.data(), p_Data.size(), 1, s_File);

	fclose(s_File);

	return true;
}

bool WinBackend::EraseFile(const std::string& p_Path)
{
	if (m_BasePath == "" || !m_CanWrite)
		return false;

	std::string s_Path = m_BasePath + p_Path;
	s_Path = Util::Utils::StringReplace(s_Path, "/", "\\");
	s_Path = Util::Utils::StringReplace(s_Path, "\\\\", "\\");

	char s_CanonicalPath[MAX_PATH];
	if (!PathCanonicalizeA(s_CanonicalPath, s_Path.c_str()))
		return false;

	if (!Util::Utils::StringStartWith(s_CanonicalPath + std::string("\\"), m_BasePath))
		return false;

	DWORD s_Attributes = GetFileAttributes(s_CanonicalPath);

	if (!(s_Attributes != INVALID_FILE_ATTRIBUTES && !(s_Attributes & FILE_ATTRIBUTE_DIRECTORY)))
		return false;

	return DeleteFileA(s_Path.c_str()) != 0;
}

bool WinBackend::GetFiles(const std::string& p_Path, std::vector<std::string>& p_Files, const std::string& p_Match, bool p_Recursive)
{
	if (m_BasePath == "" || !m_CanWrite)
		return false;

	std::string s_Path = m_BasePath + p_Path;
	s_Path = Util::Utils::StringReplace(s_Path, "/", "\\");
	s_Path = Util::Utils::StringReplace(s_Path, "\\\\", "\\");

	char s_CanonicalPath[MAX_PATH];
	if (!PathCanonicalizeA(s_CanonicalPath, s_Path.c_str()))
		return false;

	if (!Util::Utils::StringStartWith(s_CanonicalPath + std::string("\\"), m_BasePath))
		return false;

	DWORD s_FileType = GetFileAttributesA(s_CanonicalPath);

	if (s_FileType == INVALID_FILE_ATTRIBUTES)
		return false;

	if (!(s_FileType & FILE_ATTRIBUTE_DIRECTORY))
		return false;

	if (s_Path.back() == '\\')
		s_Path = s_Path.substr(0, s_Path.size() - 1);

	HANDLE s_Handle = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA s_FindData;

	if (!p_Recursive)
	{
		s_Path += "\\" + p_Match;

		s_Handle = FindFirstFileA(s_Path.c_str(), &s_FindData);

		if (INVALID_HANDLE_VALUE == s_Handle)
			return true;

		do
		{
			if (s_FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				continue;

			p_Files.push_back(s_FindData.cFileName);
		} while (FindNextFile(s_Handle, &s_FindData) != 0);

		FindClose(s_Handle);

		return true;
	}

	std::string s_Spec;
	std::stack<std::string> s_Directories;

	s_Directories.push(s_Path);

	while (!s_Directories.empty())
	{
		s_Path = s_Directories.top();
		s_Spec = s_Path + "\\" + p_Match;
		s_Directories.pop();

		s_Handle = FindFirstFileA(s_Spec.c_str(), &s_FindData);

		if (s_Handle == INVALID_HANDLE_VALUE)
			return true;

		do
		{
			if (strcmp(s_FindData.cFileName, ".") != 0 && strcmp(s_FindData.cFileName, "..") != 0)
			{
				if (s_FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					s_Directories.push(s_Path + "\\" + s_FindData.cFileName);
				}
				else
				{
					std::string s_FileName = s_FindData.cFileName;
					s_FileName = Util::Utils::StringReplace(s_FileName, "\\", "/");
					p_Files.push_back(s_FileName);
				}
			}
		} while (FindNextFileA(s_Handle, &s_FindData) != 0);

		if (GetLastError() != ERROR_NO_MORE_FILES)
		{
			FindClose(s_Handle);
			return true;
		}

		FindClose(s_Handle);
		s_Handle = INVALID_HANDLE_VALUE;
	}

	return true;
}

bool WinBackend::GetNativePath(const std::string& p_Path, std::string& p_NativePath)
{
	if (m_BasePath == "")
		return false;

	std::string s_Path = m_BasePath + p_Path;
	s_Path = Util::Utils::StringReplace(s_Path, "/", "\\");
	s_Path = Util::Utils::StringReplace(s_Path, "\\\\", "\\");

	char s_CanonicalPath[MAX_PATH];
	if (!PathCanonicalizeA(s_CanonicalPath, s_Path.c_str()))
		return false;

	if (!Util::Utils::StringStartWith(s_CanonicalPath + std::string("\\"), m_BasePath))
		return false;

	DWORD s_Attributes = GetFileAttributes(s_CanonicalPath);

	if (s_Attributes != INVALID_FILE_ATTRIBUTES && !(s_Attributes & FILE_ATTRIBUTE_DIRECTORY))
	{
		p_NativePath = s_CanonicalPath;
		return true;
	}

	return false;
}

bool WinBackend::GetFileSize(const std::string& p_Path, size_t& p_Size)
{
	if (m_BasePath == "")
		return false;

	std::string s_Path = m_BasePath + p_Path;
	s_Path = Util::Utils::StringReplace(s_Path, "/", "\\");
	s_Path = Util::Utils::StringReplace(s_Path, "\\\\", "\\");

	char s_CanonicalPath[MAX_PATH];
	if (!PathCanonicalizeA(s_CanonicalPath, s_Path.c_str()))
		return false;

	if (!Util::Utils::StringStartWith(s_CanonicalPath + std::string("\\"), m_BasePath))
		return false;

	DWORD s_Attributes = GetFileAttributes(s_CanonicalPath);

	// Check if file exists and is not a directory.
	if (!(s_Attributes != INVALID_FILE_ATTRIBUTES && !(s_Attributes & FILE_ATTRIBUTE_DIRECTORY)))
		return false;

	FILE* s_File = fopen(s_Path.c_str(), "rb");

	if (s_File == NULL)
		return false;

	// Find the size of the file.
#ifdef _WIN64
	_fseeki64(s_File, 0, SEEK_END);
	size_t s_Length = _ftelli64(s_File);
	_fseeki64(s_File, 0, SEEK_SET);
#else
	fseek(s_File, 0, SEEK_END);
	size_t s_Length = ftell(s_File);
	fseek(s_File, 0, SEEK_SET);
#endif

	p_Size = s_Length;

	fclose(s_File);

	return true;
}

bool WinBackend::DirectoryExists(const std::string& p_Path)
{
	if (m_BasePath == "")
		return false;

	std::string s_Path = m_BasePath + p_Path;
	s_Path = Util::Utils::StringReplace(s_Path, "/", "\\");
	s_Path = Util::Utils::StringReplace(s_Path, "\\\\", "\\");

	char s_CanonicalPath[MAX_PATH];
	if (!PathCanonicalizeA(s_CanonicalPath, s_Path.c_str()))
		return false;

	if (!Util::Utils::StringStartWith(s_CanonicalPath + std::string("\\"), m_BasePath))
		return false;

	DWORD s_Attributes = GetFileAttributes(s_CanonicalPath);

	return (s_Attributes != INVALID_FILE_ATTRIBUTES && (s_Attributes & FILE_ATTRIBUTE_DIRECTORY));
}
#endif