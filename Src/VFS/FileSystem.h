#pragma once

#include <Misc/StdAfx.h>

#include <vector>
#include <unordered_map>

namespace VFS
{
	class FSFile;
	class FSBackend;

	class FileSystem
	{
	public:
		static FileSystem* GetInstance();

	private:
		static FileSystem* m_Instance;

	public:
		FileSystem();
		~FileSystem();

	public:
		void Init(const std::string& p_BasePath);

		/// Mounts the given FSBackend to the specified mount point.
		/// Mount points should consist of only one path component.
		void Mount(FSBackend* p_Backend, const std::string& p_MountPoint);

		/// Dismounts a FSBackend from the specified mount point 
		/// (if one exists).
		void Dismount(const std::string& p_MountPoint);

		// Gets the FSBackend mounted to the specified path
		// (if it exists).
		FSBackend* GetBackend(const std::string& p_Path);

	public:
		bool FileExists(const std::string& p_Path);
		bool ReadFile(const std::string& p_Path, std::string& p_Data);
		bool WriteFile(const std::string& p_Path, const std::string& p_Data, bool p_Append);
		bool OpenFile(const std::string& p_Path, const std::string& p_Mode, FSFile** p_File);
		bool CloseFile(FSFile* p_File);
		bool EraseFile(const std::string& p_Path);
		bool GetFiles(const std::string& p_Path, std::vector<std::string>& p_Files, const std::string& p_Match = "*", bool p_Recursive = false);
		bool GetNativePath(const std::string& p_Path, std::string& p_NativePath);
		bool GetFileSize(const std::string& p_Path, size_t& p_Size);
		bool DirectoryExists(const std::string& p_Path);

	private:
		std::unordered_map<uint32_t, FSBackend*> m_MountPoints;
	};
}