#ifndef _WIN32
#include "PosixBackend.h"

#include <Util/Utils.h>
#include <VFS/Files/NativeFile.h>

#include <stack>
#include <linux/limits.h>
#include <sys/stat.h>

using namespace VFS;
using namespace VFS::Backends;

PosixBackend::PosixBackend(const std::string& p_BasePath, bool p_CanWrite, bool p_CanDelete, bool p_ForceCreate) :
	m_BasePath(p_BasePath), 
	m_CanWrite(p_CanWrite), 
	m_CanDelete(p_CanDelete)
{
	m_BasePath = Util::Utils::StringTrimRight(m_BasePath, '/');
	m_BasePath = Util::Utils::StringTrimRight(m_BasePath, '\\');

	m_BasePath = Util::Utils::StringReplace(m_BasePath, "\\", "/");
	m_BasePath = Util::Utils::StringReplace(m_BasePath, "//", "/");

	m_BasePath += "/";

    // TODO: Create folder if it doesn't exist.
	/*DWORD s_Attributes = GetFileAttributes(m_BasePath.c_str());

	// Base path doesn't exist
	if (!(s_Attributes != INVALID_FILE_ATTRIBUTES && (s_Attributes & FILE_ATTRIBUTE_DIRECTORY)))
	{
		if (p_ForceCreate && SHCreateDirectoryExA(NULL, m_BasePath.c_str(), NULL) == ERROR_SUCCESS)
			return;

		m_BasePath = "";
	}*/
}

PosixBackend::~PosixBackend()
{

}

bool PosixBackend::FileExists(const std::string& p_Path)
{
	if (m_BasePath == "")
		return false;

	std::string s_Path = m_BasePath + p_Path;
	s_Path = Util::Utils::StringReplace(s_Path, "\\", "/");
	s_Path = Util::Utils::StringReplace(s_Path, "//", "/");

	char s_CanonicalPath[PATH_MAX];
	if (!realpath(s_Path.c_str(), s_CanonicalPath))
		return false;

	if (!Util::Utils::StringStartWith(s_CanonicalPath + std::string("/"), m_BasePath))
		return false;

	struct stat s_Stat;
	return (stat(s_CanonicalPath, &s_Stat) == 0 && s_Stat.st_mode & S_IFREG);
}

bool PosixBackend::ReadFile(const std::string& p_Path, std::string& p_Data)
{
	if (m_BasePath == "")
		return false;

	std::string s_Path = m_BasePath + p_Path;
	s_Path = Util::Utils::StringReplace(s_Path, "\\", "/");
	s_Path = Util::Utils::StringReplace(s_Path, "//", "/");

	char s_CanonicalPath[PATH_MAX];
	if (!realpath(s_Path.c_str(), s_CanonicalPath))
		return false;

	if (!Util::Utils::StringStartWith(s_CanonicalPath + std::string("/"), m_BasePath))
		return false;

	// Check if file exists and is not a directory.
	struct stat s_Stat;
	if (stat(s_CanonicalPath, &s_Stat) != 0 || !(s_Stat.st_mode & S_IFREG))
		return false;

	FILE* s_File = fopen(s_Path.c_str(), "rb");

	if (s_File == NULL)
		return false;

	// Find the size of the file.
	fseek(s_File, 0, SEEK_END);
	size_t s_Length = (size_t) ftell(s_File);
	fseek(s_File, 0, SEEK_SET);

	// Read all the file data.
	char* s_Data = new char[s_Length];
	size_t s_ReadLength = fread(s_Data, 1, s_Length, s_File);

	p_Data = std::string(s_Data, s_ReadLength);
	free(s_Data);

	fclose(s_File);

	return true;
}

bool PosixBackend::OpenFile(const std::string& p_Path, const std::string& p_Mode, FSFile** p_File)
{
	if (m_BasePath == "")
		return false;

	std::string s_Path = m_BasePath + p_Path;
	s_Path = Util::Utils::StringReplace(s_Path, "\\", "/");
	s_Path = Util::Utils::StringReplace(s_Path, "//", "/");

	char s_CanonicalPath[PATH_MAX];
	if (!realpath(s_Path.c_str(), s_CanonicalPath))
		return false;

	if (!Util::Utils::StringStartWith(s_CanonicalPath + std::string("/"), m_BasePath))
		return false;

	FILE* s_File = fopen(s_Path.c_str(), p_Mode.c_str());

	if (s_File == NULL)
		return false;

	*p_File = new VFS::Files::NativeFile(s_File);

	return true;
}

bool PosixBackend::WriteFile(const std::string& p_Path, const std::string& p_Data, bool p_Append)
{
	if (m_BasePath == "" || !m_CanWrite)
		return false;

	std::string s_Path = m_BasePath + p_Path;
	s_Path = Util::Utils::StringReplace(s_Path, "\\", "/");
	s_Path = Util::Utils::StringReplace(s_Path, "//", "/");

	char s_CanonicalPath[PATH_MAX];
	if (!realpath(s_Path.c_str(), s_CanonicalPath))
		return false;

	if (!Util::Utils::StringStartWith(s_CanonicalPath + std::string("/"), m_BasePath))
		return false;

	std::string s_PathString = s_CanonicalPath;

	std::string s_Directory;
	const size_t s_LastIndex = s_PathString.rfind('\\');

	if (std::string::npos != s_LastIndex)
		s_Directory = s_PathString.substr(0, s_LastIndex);

	mkdir(s_Directory.c_str(), 0777);

	FILE* s_File = fopen(s_CanonicalPath, p_Append ? "ab" : "wb");

	if (s_File == NULL)
		return false;

	fwrite(p_Data.data(), p_Data.size(), 1, s_File);

	fclose(s_File);

	return true;
}

bool PosixBackend::EraseFile(const std::string& p_Path)
{
	if (m_BasePath == "" || !m_CanWrite)
		return false;

	std::string s_Path = m_BasePath + p_Path;
	s_Path = Util::Utils::StringReplace(s_Path, "\\", "/");
	s_Path = Util::Utils::StringReplace(s_Path, "//", "/");

	char s_CanonicalPath[PATH_MAX];
	if (!realpath(s_Path.c_str(), s_CanonicalPath))
		return false;

	if (!Util::Utils::StringStartWith(s_CanonicalPath + std::string("/"), m_BasePath))
		return false;

	// Check if file exists and is not a directory.
	struct stat s_Stat;
	if (stat(s_CanonicalPath, &s_Stat) != 0 || !(s_Stat.st_mode & S_IFREG))
		return false;

	return remove(s_Path.c_str()) == 0;
}

bool PosixBackend::GetFiles(const std::string& p_Path, std::vector<std::string>& p_Files, const std::string& p_Match, bool p_Recursive)
{
	if (m_BasePath == "" || !m_CanWrite)
		return false;

	std::string s_Path = m_BasePath + p_Path;
	s_Path = Util::Utils::StringReplace(s_Path, "\\", "/");
	s_Path = Util::Utils::StringReplace(s_Path, "//", "/");

	char s_CanonicalPath[PATH_MAX];
	if (!realpath(s_Path.c_str(), s_CanonicalPath))
		return false;

	if (!Util::Utils::StringStartWith(s_CanonicalPath + std::string("/"), m_BasePath))
		return false;

	// TODO: Implement this.
	return false;
}

bool PosixBackend::GetNativePath(const std::string& p_Path, std::string& p_NativePath)
{
	if (m_BasePath == "")
		return false;

	std::string s_Path = m_BasePath + p_Path;
	s_Path = Util::Utils::StringReplace(s_Path, "\\", "/");
	s_Path = Util::Utils::StringReplace(s_Path, "//", "/");

	char s_CanonicalPath[PATH_MAX];
	if (!realpath(s_Path.c_str(), s_CanonicalPath))
		return false;

	if (!Util::Utils::StringStartWith(s_CanonicalPath + std::string("/"), m_BasePath))
		return false;

	// Check if file exists and is not a directory.
	struct stat s_Stat;
	if (stat(s_CanonicalPath, &s_Stat) != 0 || !(s_Stat.st_mode & S_IFREG))
		return false;

	p_NativePath = s_CanonicalPath;
	return true;
}

bool PosixBackend::GetFileSize(const std::string& p_Path, size_t& p_Size)
{
	if (m_BasePath == "")
		return false;

	std::string s_Path = m_BasePath + p_Path;
	s_Path = Util::Utils::StringReplace(s_Path, "\\", "/");
	s_Path = Util::Utils::StringReplace(s_Path, "//", "/");

	char s_CanonicalPath[PATH_MAX];
	if (!realpath(s_Path.c_str(), s_CanonicalPath))
		return false;

	if (!Util::Utils::StringStartWith(s_CanonicalPath + std::string("/"), m_BasePath))
		return false;

	// Check if file exists and is not a directory.
	struct stat s_Stat;
	if (stat(s_CanonicalPath, &s_Stat) != 0 || !(s_Stat.st_mode & S_IFREG))
		return false;

	FILE* s_File = fopen(s_Path.c_str(), "rb");

	if (s_File == NULL)
		return false;

	// Find the size of the file.
	fseek(s_File, 0, SEEK_END);
	size_t s_Length = (size_t) ftell(s_File);
	fseek(s_File, 0, SEEK_SET);

	p_Size = s_Length;

	fclose(s_File);

	return true;
}

bool PosixBackend::DirectoryExists(const std::string& p_Path)
{
	if (m_BasePath == "")
		return false;

	std::string s_Path = m_BasePath + p_Path;
	s_Path = Util::Utils::StringReplace(s_Path, "\\", "/");
	s_Path = Util::Utils::StringReplace(s_Path, "//", "/");

	char s_CanonicalPath[PATH_MAX];
	if (!realpath(s_Path.c_str(), s_CanonicalPath))
		return false;

	if (!Util::Utils::StringStartWith(s_CanonicalPath + std::string("/"), m_BasePath))
		return false;

	struct stat s_Stat;
	return (stat(s_CanonicalPath, &s_Stat) == 0 && s_Stat.st_mode & S_IFDIR);
}

#endif