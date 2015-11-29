#include "Log.h"

#include <ctime>
#include <sstream>

#include <VFS/FileSystem.h>

#ifndef _WIN32
#include <time.h>

int _vscprintf (const char * format, va_list pargs)
{
	int retval;
	va_list argcopy;
	va_copy(argcopy, pargs);
	retval = vsnprintf(NULL, 0, format, argcopy);
	va_end(argcopy);
	return retval;
}

#define vsprintf_s vsnprintf
#endif

using namespace Util;

Log* Log::m_Instance = NULL;

Log* Log::GetInstance()
{
	if (!m_Instance)
		m_Instance = new Log();

	return m_Instance;
}

Log::Log() : 
	m_LogLevel(LogLevel::None),
	m_ProcessingQueue(false),
	m_OutputFile(nullptr)
{
}

Log::~Log()
{
	ProcessQueue();
	VFS::FileSystem::GetInstance()->CloseFile(m_OutputFile);
}

void Log::SetConsoleColor(ConsoleColor::type p_Color)
{
#ifdef _WIN32
	HANDLE s_Screen = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(s_Screen, p_Color);
#endif
    // TODO: Add Linux support for this?!
}

void Log::Init(LogLevel::type p_LogLevel, const std::string& p_OutputFile /* = "" */, bool p_Append /* = true */)
{
	m_LogLevel = p_LogLevel;

	if (p_OutputFile == "")
		return;

	if (p_Append)
	{
		VFS::FileSystem::GetInstance()->OpenFile("/logs/" + p_OutputFile + ".log", "a+", &m_OutputFile);
	}
	else
	{
		int i = 0;
		char s_Filename[2048];

		while (true)
		{
			sprintf(s_Filename, "/logs/%s_%04d.log", p_OutputFile.c_str(), i);

			if (!VFS::FileSystem::GetInstance()->FileExists(s_Filename))
				break;

			++i;
		}

		VFS::FileSystem::GetInstance()->OpenFile(s_Filename, "w+", &m_OutputFile);
	}

	std::time_t s_RawTime;
	std::tm* s_TimeInfo;
	char s_TimeBuffer[80];

	std::time(&s_RawTime);
	s_TimeInfo = std::localtime(&s_RawTime);

	std::strftime(s_TimeBuffer, 80, "%Y-%m-%d %H:%M:%S", s_TimeInfo);

	if (m_OutputFile)
	{
		char s_Splitter[512];
		sprintf(s_Splitter, "[%s] -------------------------------------------------------\n", s_TimeBuffer);

		char s_StartTime[512];
		sprintf(s_StartTime, "[%s] Starting log at %s\n", s_TimeBuffer, s_TimeBuffer);

		m_OutputFile->Write(s_Splitter);
		m_OutputFile->Write(s_StartTime);
		m_OutputFile->Write(s_Splitter);

		m_OutputFile->Flush();
	}
}

void Log::Write(LogLevel::type p_LogLevel, const char* p_File, int p_Line, const char* p_Function, const char* p_Text, ...)
{
	EasyLockScopeGuard s_Guard(&m_Lock);

	if (p_LogLevel > m_LogLevel)
		return;

	va_list s_ArgList;

	va_start(s_ArgList, p_Text);

	int s_StringLength = _vscprintf(p_Text, s_ArgList) + 1;
	char* s_LogOutput = (char*) malloc(s_StringLength * sizeof(char));

	vsprintf_s(s_LogOutput, s_StringLength, p_Text, s_ArgList);

	va_end(s_ArgList);

	std::time_t s_RawTime;
	std::tm* s_TimeInfo;

	std::time(&s_RawTime);
	s_TimeInfo = std::localtime(&s_RawTime);

	char* s_TimeStr = (char*) malloc(9 * sizeof(char));
	std::strftime(s_TimeStr, 9, "%H:%M:%S", s_TimeInfo);

	char s_TimeBuffer[80];
	std::strftime(s_TimeBuffer, 80, "%Y-%m-%d %H:%M:%S", s_TimeInfo);

	char* s_LongTimeStr = (char*) malloc((strlen(s_TimeBuffer) + 1) * sizeof(char*));
	strcpy(s_LongTimeStr, s_TimeBuffer);

	LogItem s_LogItem;

	s_LogItem.m_LogLevel = p_LogLevel;
	s_LogItem.m_SourceFile = p_File;
	s_LogItem.m_SourceFunction = p_Function;
	s_LogItem.m_SourceLine = p_Line;
	s_LogItem.m_Time = s_TimeStr;
	s_LogItem.m_LongTime = s_LongTimeStr;
	s_LogItem.m_Text = s_LogOutput;

	m_QueuedLogItems.push(s_LogItem);

	ProcessQueue();
}

void Log::ProcessQueue()
{
	if (m_ProcessingQueue)
		return;

	m_ProcessingQueue = true;

	while (!m_QueuedLogItems.empty())
	{
		auto s_LogItem = m_QueuedLogItems.front();
		m_QueuedLogItems.pop();

		SetConsoleColor(ConsoleColor::LightGrey);
		printf("[%s] ", s_LogItem.m_Time);
		SetConsoleColor(ConsoleColor::Green);
		printf(s_LogItem.m_SourceFile);
		SetConsoleColor(ConsoleColor::Brown);
		printf(" (%d) ", s_LogItem.m_SourceLine);
		SetConsoleColor(ConsoleColor::Cyan);
		printf("[%s]", s_LogItem.m_SourceFunction);
		SetConsoleColor(ConsoleColor::LightGrey);
		printf("\n>> ");

		switch (s_LogItem.m_LogLevel)
		{
		case LogLevel::Info:
			SetConsoleColor(ConsoleColor::White);
			break;
		case LogLevel::Warn:
			SetConsoleColor(ConsoleColor::Yellow);
			break;
		case LogLevel::Error:
			SetConsoleColor(ConsoleColor::LightRed);
			break;
		case LogLevel::Data:
			SetConsoleColor(ConsoleColor::LightGreen);
			break;
		case LogLevel::Debug:
			SetConsoleColor(ConsoleColor::LightCyan);
			break;
		default:
			SetConsoleColor(ConsoleColor::White);
			break;
		}

		printf("%s\n", s_LogItem.m_Text);

		SetConsoleColor(ConsoleColor::White);

		if (m_OutputFile)
		{
			// Print to file
			char s_Buffer[16384];
			sprintf(s_Buffer, "[%s] %s (%d) [%s] >> %s\n", s_LogItem.m_LongTime, s_LogItem.m_SourceFile, s_LogItem.m_SourceLine, s_LogItem.m_SourceFunction, s_LogItem.m_Text);

			m_OutputFile->Write(s_Buffer);
			m_OutputFile->Flush();
		}

		free((void*) s_LogItem.m_Text);
		free((void*) s_LogItem.m_LongTime);
		free((void*) s_LogItem.m_Time);
	}

	m_ProcessingQueue = false;
}