#pragma once

#include "EasyLock.h"
#include <queue>
#include <string>

// We only want the logging function if we're in debug mode
// Otherwise it should be nothing
#define Logger(x, y, ...) Util::Log::GetInstance()->Write(x, __FILE__, __LINE__, __FUNCTION__, y, __VA_ARGS__);

namespace VFS
{
	class FSFile;
}

namespace Util
{
	namespace LogLevel
	{
		enum type
		{
			None,
			Info,
			Error,
			Warn,
			Data,
			Debug,
			All
		};
	};

	namespace ConsoleColor
	{
		enum type
		{
			Black,
			Blue,
			Green,
			Cyan,
			Red,
			Magenta,
			Brown,
			LightGrey,
			DarkGrey,
			LightBlue,
			LightGreen,
			LightCyan,
			LightRed,
			LightMagenta,
			Yellow,
			White,
			Blink
		};
	};

	struct LogItem
	{
		LogLevel::type m_LogLevel;
		const char* m_SourceFile;
		int m_SourceLine;
		const char* m_SourceFunction;
		const char* m_Text;
		const char* m_Time;
		const char* m_LongTime;
	};

	class Log
	{
	protected:
		static Log* m_Instance;

	public:
		static Log* GetInstance();

	public:
		Log();
		~Log();

	public:
		void Init(LogLevel::type p_LogLevel, const std::string& p_OutputFile = "", bool p_Append = true);
		void Write(LogLevel::type p_LogLevel, char* p_File, int p_Line, char* p_Function, char* p_Text, ...);

	protected:
		void SetConsoleColor(ConsoleColor::type p_Color);
		void ProcessQueue();

	protected:
		LogLevel::type m_LogLevel;
		std::queue<LogItem> m_QueuedLogItems;
		EasyLock m_Lock;
		bool m_ProcessingQueue;
		VFS::FSFile* m_OutputFile;
	};

}