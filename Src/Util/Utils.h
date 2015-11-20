#pragma once

#include <Misc/StdAfx.h>

namespace Util
{
	class Utils
	{
	public:
		static std::string StringReplace(const std::string& p_String, const std::string& p_From, const std::string& p_To);
		static uint32_t StringHash(const std::string& p_String);
		static std::string StringTrimLeft(const std::string& p_String, char p_Character);
		static std::string StringTrimRight(const std::string& p_String, char p_Character);
		static std::string StringTrim(const std::string& p_String, char p_Character);
		static bool StringStartWith(const std::string& p_String, const std::string& p_Test);
		static void AllocateConsole(const std::string& p_Title);
		static std::string GetCurrentDir();
	};
}