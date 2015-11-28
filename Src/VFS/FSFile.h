#pragma once

#include <Misc/StdAfx.h>

namespace VFS
{
	class FSFile
	{
	public:
		virtual ~FSFile() {};

	public:
		virtual size_t Write(const std::string& p_Data, int64_t p_Length = -1) = 0;
		virtual std::string Read(size_t p_Length) = 0;
		virtual int Scan(const char* p_Format, ...) = 0;
		virtual void Seek(int64_t p_Offset, int p_Origin) = 0;

		virtual void Close() = 0;
		virtual void Flush() = 0;
	};
}