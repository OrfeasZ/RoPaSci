#include "FileSystem.h"

#include "FSBackend.h"
#include "FSFile.h"
#include "Backends/WinBackend.h"

#include <Util/Utils.h>

#include <vector>
#include <fstream>

using namespace VFS;

FileSystem* FileSystem::m_Instance = nullptr;

FileSystem* FileSystem::GetInstance()
{
	if (m_Instance == nullptr)
		m_Instance = new FileSystem();
	return m_Instance;
}

FileSystem::FileSystem()
{

}

FileSystem::~FileSystem()
{
	for (auto it = m_MountPoints.begin(); it != m_MountPoints.end(); ++it)
		delete it->second;

	m_MountPoints.clear();
}

void FileSystem::Init(const std::string& p_BasePath)
{
	// TODO: Linux support
	// Mount the base filesystem to "/native".
	Mount(new Backends::WinBackend(p_BasePath), "/native");

	// Mount the data folder to "/data".
	Mount(new Backends::WinBackend(p_BasePath + "\\data"), "/data");

	// Mount the logs folder to "/logs".
	Mount(new Backends::WinBackend(p_BasePath + "\\logs", true, true, true), "/logs");
}

void FileSystem::Mount(FSBackend* p_Backend, const std::string& p_MountPoint)
{
	// Do we already have a backend assigned to this mount point?
	auto s_Backend = m_MountPoints.find(Util::Utils::StringHash(p_MountPoint));

	// Apparently, we do.
	if (s_Backend != m_MountPoints.end())
	{
		// Destroy it.
		delete s_Backend->second;
	}

	// Mount the new point.
	m_MountPoints[Util::Utils::StringHash(p_MountPoint)] = p_Backend;
}

void FileSystem::Dismount(const std::string& p_MountPoint)
{
	// Do we have a backend assigned to this mount point?
	auto s_Backend = m_MountPoints.find(Util::Utils::StringHash(p_MountPoint));

	// Guess not.
	if (s_Backend == m_MountPoints.end())
		return;

	// Destroy and dismount it.
	delete s_Backend->second;
	m_MountPoints.erase(s_Backend);
}

FSBackend* FileSystem::GetBackend(const std::string& p_Path)
{
	// Split path.
	std::string s_Delim("/");
	std::string s_InputPath = Util::Utils::StringReplace(p_Path, "\\", "/");
	s_InputPath = Util::Utils::StringReplace(s_InputPath, "//", "/");

	auto s_Split = s_InputPath.substr(1).find(s_Delim);

	std::string s_Base = s_InputPath;

	if (s_Split != std::string::npos)
		s_Base = s_InputPath.substr(0, s_Split + 1);

	// Does a backend exist at the given mount point?
	auto s_BackendIt = m_MountPoints.find(Util::Utils::StringHash(s_Base));

	if (s_BackendIt == m_MountPoints.end())
		return nullptr;

	return s_BackendIt->second;
}

bool FileSystem::FileExists(const std::string& p_Path)
{
	// Split path.
	std::string s_Delim("/");
	std::string s_InputPath = Util::Utils::StringReplace(p_Path, "\\", "/");
	s_InputPath = Util::Utils::StringReplace(s_InputPath, "//", "/");

	auto s_Split = s_InputPath.substr(1).find(s_Delim);

	if (s_Split == std::string::npos)
		return false;

	std::string s_Base = s_InputPath.substr(0, s_Split + 1);
	std::string s_Path = s_InputPath.substr(s_Split + 1 + (s_Delim.size()));

	// Do we have a file path?
	if (s_Path.size() == 0)
		return false;

	// Does a backend exist at the given mount point?
	auto s_BackendIt = m_MountPoints.find(Util::Utils::StringHash(s_Base));

	if (s_BackendIt == m_MountPoints.end())
		return false;

	// Does the backend contain that file?
	return s_BackendIt->second->FileExists(s_Path);
}

bool FileSystem::ReadFile(const std::string& p_Path, std::string& p_Data)
{
	// Split path.
	std::string s_Delim("/");
	std::string s_InputPath = Util::Utils::StringReplace(p_Path, "\\", "/");
	s_InputPath = Util::Utils::StringReplace(s_InputPath, "//", "/");

	auto s_Split = s_InputPath.substr(1).find(s_Delim);

	if (s_Split == std::string::npos)
		return false;

	std::string s_Base = s_InputPath.substr(0, s_Split + 1);
	std::string s_Path = s_InputPath.substr(s_Split + 1 + (s_Delim.size()));

	// Do we have a file path?
	if (s_Path.size() == 0)
		return false;

	// Does a backend exist at the given mount point?
	auto s_BackendIt = m_MountPoints.find(Util::Utils::StringHash(s_Base));

	if (s_BackendIt == m_MountPoints.end())
	{
		return false;
	}

	// Does the backend contain that file?
	if (!s_BackendIt->second->FileExists(s_Path))
	{
		return false;
	}

	return s_BackendIt->second->ReadFile(s_Path, p_Data);
}

bool FileSystem::OpenFile(const std::string& p_Path, const std::string& p_Mode, FSFile** p_File)
{
	// Split path.
	std::string s_Delim("/");
	std::string s_InputPath = Util::Utils::StringReplace(p_Path, "\\", "/");
	s_InputPath = Util::Utils::StringReplace(s_InputPath, "//", "/");

	auto s_Split = s_InputPath.substr(1).find(s_Delim);

	if (s_Split == std::string::npos)
		return false;

	std::string s_Base = s_InputPath.substr(0, s_Split + 1);
	std::string s_Path = s_InputPath.substr(s_Split + 1 + (s_Delim.size()));

	// Do we have a file path?
	if (s_Path.size() == 0)
		return false;

	// Does a backend exist at the given mount point?
	auto s_BackendIt = m_MountPoints.find(Util::Utils::StringHash(s_Base));

	if (s_BackendIt == m_MountPoints.end())
	{
		return false;
	}

	return s_BackendIt->second->OpenFile(s_Path, p_Mode, p_File);
}

bool FileSystem::CloseFile(FSFile* p_File)
{
	if (!p_File)
		return false;

	delete p_File;
	return true;
}

bool FileSystem::WriteFile(const std::string& p_Path, const std::string& p_Data, bool p_Append)
{
	// Split path.
	std::string s_Delim("/");
	std::string s_InputPath = Util::Utils::StringReplace(p_Path, "\\", "/");
	s_InputPath = Util::Utils::StringReplace(s_InputPath, "//", "/");

	auto s_Split = s_InputPath.substr(1).find(s_Delim);

	if (s_Split == std::string::npos)
		return false;

	std::string s_Base = s_InputPath.substr(0, s_Split + 1);
	std::string s_Path = s_InputPath.substr(s_Split + 1 + (s_Delim.size()));

	// Do we have a file path?
	if (s_Path.size() == 0)
		return false;

	// Does a backend exist at the given mount point?
	auto s_BackendIt = m_MountPoints.find(Util::Utils::StringHash(s_Base));

	if (s_BackendIt == m_MountPoints.end())
		return false;

	return s_BackendIt->second->WriteFile(s_Path, p_Data, p_Append);
}

bool FileSystem::EraseFile(const std::string& p_Path)
{
	// Split path.
	std::string s_Delim("/");
	std::string s_InputPath = Util::Utils::StringReplace(p_Path, "\\", "/");
	s_InputPath = Util::Utils::StringReplace(s_InputPath, "//", "/");

	auto s_Split = s_InputPath.substr(1).find(s_Delim);

	if (s_Split == std::string::npos)
		return false;

	std::string s_Base = s_InputPath.substr(0, s_Split + 1);
	std::string s_Path = s_InputPath.substr(s_Split + 1 + (s_Delim.size()));

	// Do we have a file path?
	if (s_Path.size() == 0)
		return false;

	// Does a backend exist at the given mount point?
	auto s_BackendIt = m_MountPoints.find(Util::Utils::StringHash(s_Base));

	if (s_BackendIt == m_MountPoints.end())
		return false;

	// Does the backend contain that file?
	if (!s_BackendIt->second->FileExists(s_Path))
		return false;

	return s_BackendIt->second->EraseFile(s_Path);
}

bool FileSystem::GetFiles(const std::string& p_Path, std::vector<std::string>& p_Files, const std::string& p_Match, bool p_Recursive)
{
	// Split path.
	std::string s_Delim("/");
	std::string s_InputPath = Util::Utils::StringReplace(p_Path, "\\", "/");
	s_InputPath = Util::Utils::StringReplace(s_InputPath, "//", "/");

	auto s_Split = s_InputPath.substr(1).find(s_Delim);

	if (s_Split == std::string::npos)
		return false;

	std::string s_Base = s_InputPath.substr(0, s_Split + 1);
	std::string s_Path = s_InputPath.substr(s_Split + 1 + (s_Delim.size()));

	// Do we have a path?
	if (s_Path.size() == 0)
		return false;

	// Does a backend exist at the given mount point?
	auto s_BackendIt = m_MountPoints.find(Util::Utils::StringHash(s_Base));

	if (s_BackendIt == m_MountPoints.end())
		return false;

	return s_BackendIt->second->GetFiles(s_Path, p_Files, p_Match, p_Recursive);
}

bool FileSystem::GetNativePath(const std::string& p_Path, std::string& p_NativePath)
{
	// Split path.
	std::string s_Delim("/");
	std::string s_InputPath = Util::Utils::StringReplace(p_Path, "\\", "/");
	s_InputPath = Util::Utils::StringReplace(s_InputPath, "//", "/");

	auto s_Split = s_InputPath.substr(1).find(s_Delim);

	if (s_Split == std::string::npos)
		return false;

	std::string s_Base = s_InputPath.substr(0, s_Split + 1);
	std::string s_Path = s_InputPath.substr(s_Split + 1 + (s_Delim.size()));

	// Do we have a file path?
	if (s_Path.size() == 0)
		return false;

	// Does a backend exist at the given mount point?
	auto s_BackendIt = m_MountPoints.find(Util::Utils::StringHash(s_Base));

	if (s_BackendIt == m_MountPoints.end())
		return false;

	// Does the backend contain that file?
	return s_BackendIt->second->GetNativePath(s_Path, p_NativePath);
}

bool FileSystem::GetFileSize(const std::string& p_Path, size_t& p_Size)
{
	// Split path.
	std::string s_Delim("/");
	std::string s_InputPath = Util::Utils::StringReplace(p_Path, "\\", "/");
	s_InputPath = Util::Utils::StringReplace(s_InputPath, "//", "/");

	auto s_Split = s_InputPath.substr(1).find(s_Delim);

	if (s_Split == std::string::npos)
		return false;

	std::string s_Base = s_InputPath.substr(0, s_Split + 1);
	std::string s_Path = s_InputPath.substr(s_Split + 1 + (s_Delim.size()));

	// Do we have a file path?
	if (s_Path.size() == 0)
		return false;

	// Does a backend exist at the given mount point?
	auto s_BackendIt = m_MountPoints.find(Util::Utils::StringHash(s_Base));

	if (s_BackendIt == m_MountPoints.end())
		return false;

	// Does the backend contain that file?
	return s_BackendIt->second->GetFileSize(s_Path, p_Size);
}

bool FileSystem::DirectoryExists(const std::string& p_Path)
{
	// Split path.
	std::string s_Delim("/");
	std::string s_InputPath = Util::Utils::StringReplace(p_Path, "\\", "/");
	s_InputPath = Util::Utils::StringReplace(s_InputPath, "//", "/");

	auto s_Split = s_InputPath.substr(1).find(s_Delim);

	if (s_Split == std::string::npos)
		return false;

	std::string s_Base = s_InputPath.substr(0, s_Split + 1);
	std::string s_Path = s_InputPath.substr(s_Split + 1 + (s_Delim.size()));

	// Do we have a file path?
	if (s_Path.size() == 0)
		return false;

	// Does a backend exist at the given mount point?
	auto s_BackendIt = m_MountPoints.find(Util::Utils::StringHash(s_Base));

	if (s_BackendIt == m_MountPoints.end())
		return false;

	// Does the backend contain that directory?
	return s_BackendIt->second->DirectoryExists(s_Path);
}