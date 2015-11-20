#include "NativeFile.h"

using namespace VFS;
using namespace VFS::Files;

NativeFile::NativeFile(FILE* p_File) :
	m_File(p_File)
{
}

NativeFile::~NativeFile()
{
	if (m_File)
		fclose(m_File);

	m_File = nullptr;
}

size_t NativeFile::Write(const std::string& p_Data, int64_t p_Length /* = -1 */)
{
	if (!m_File)
		return 0;

	return fwrite(p_Data.data(), (size_t) (p_Length == -1 ? p_Data.size() : p_Length), 1, m_File);
}

std::string NativeFile::Read(size_t p_Length)
{
	if (!m_File)
		return nullptr;

	char* s_Buffer = new char[p_Length];
	size_t s_ReadSize = fread(s_Buffer, p_Length, 1, m_File);

	std::string s_StringBuffer(s_Buffer, s_ReadSize);

	delete [] s_Buffer;

	return s_StringBuffer;
}

void NativeFile::Close()
{
	if (!m_File)
		return;

	fclose(m_File);
	m_File = nullptr;
}

void NativeFile::Flush()
{
	if (!m_File)
		return;

	fflush(m_File);
}