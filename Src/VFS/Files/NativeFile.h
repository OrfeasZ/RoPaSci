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
			virtual size_t Write(const std::string& p_Data, int64_t p_Length = -1) override;
			virtual std::string Read(size_t p_Length) override;
			virtual int Scan(const char* p_Format, ...) override;
			virtual void Seek(int64_t p_Offset, int p_Origin) override;

			virtual void Close();
			virtual void Flush();

		protected:
			FILE* m_File;
		};
	}
}