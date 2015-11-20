#ifdef _WIN32
#include <VFS/FSBackend.h>

#pragma comment(lib, "Shell32.lib")

namespace VFS
{
	namespace Backends
	{
		class WinBackend : public FSBackend
		{
		public:
			WinBackend(const std::string& p_BasePath, bool p_CanWrite = true, bool p_CanDelete = true, bool p_ForceCreate = false);
			virtual ~WinBackend();

		public:
			virtual bool FileExists(const std::string& p_Path);
			virtual bool ReadFile(const std::string& p_Path, std::string& p_Data);
			virtual bool WriteFile(const std::string& p_Path, const std::string& p_Data, bool p_Append);
			virtual bool OpenFile(const std::string& p_Path, const std::string& p_Mode, FSFile** p_File);
			virtual bool EraseFile(const std::string& p_Path);
			virtual bool GetFiles(const std::string& p_Path, std::vector<std::string>& p_Files, const std::string& p_Match = "*", bool p_Recursive = false);
			virtual bool GetNativePath(const std::string& p_Path, std::string& p_NativePath);
			virtual bool GetFileSize(const std::string& p_Path, size_t& p_Size);
			virtual bool DirectoryExists(const std::string& p_Path);

		private:
			std::string m_BasePath;
			bool m_CanWrite;
			bool m_CanDelete;
		};
	}
}
#endif