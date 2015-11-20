#pragma once

#include <VFS/FSFile.h>
#include <stdio.h>

namespace VFS
{
	namespace Files
	{
		class NativeFile : public FSFile
		{
		public:
			NativeFile(FILE* p_File);
			virtual ~NativeFile() override;

		public:
			virtual size_t Write(const std::string& p_Data, int64_t p_Length = -1);
			virtual std::string Read(size_t p_Length);
			virtual void Close();
			virtual void Flush();

		protected:
			FILE* m_File;
		};
	}
}