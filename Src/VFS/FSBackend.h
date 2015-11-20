#pragma once

#include <Misc/StdAfx.h>

#include <vector>

namespace VFS
{
	class FSFile;

	class FSBackend
	{
	public:
		virtual ~FSBackend() {};

	public:
		virtual bool FileExists(const std::string& p_Path) = 0;
		virtual bool ReadFile(const std::string& p_Path, std::string& p_Data) = 0;
		virtual bool WriteFile(const std::string& p_Path, const std::string& p_Data, bool p_Append) = 0;
		virtual bool OpenFile(const std::string& p_Path, const std::string& p_Mode, FSFile** p_File) = 0;
		virtual bool EraseFile(const std::string& p_Path) = 0;
		virtual bool GetFiles(const std::string& p_Path, std::vector<std::string>& p_Files, const std::string& p_Match = "*", bool p_Recursive = false) = 0;
		virtual bool GetNativePath(const std::string& p_Path, std::string& p_NativePath) = 0;
		virtual bool GetFileSize(const std::string& p_Path, size_t& p_Size) = 0;
		virtual bool DirectoryExists(const std::string& p_Path) = 0;
	};
}