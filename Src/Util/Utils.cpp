#include "Utils.h"

#ifdef _WIN32
#include <direct.h>
#else
#include <unistd.h>
#endif

using namespace Util;

std::string Utils::StringReplace(const std::string& p_String, const std::string& p_From, const std::string& p_To)
{
	size_t s_Position = 0;
	std::string s_String(p_String);

	while ((s_Position = s_String.find(p_From, s_Position)) != std::string::npos)
	{
		s_String.replace(s_Position, p_From.length(), p_To);
		s_Position += p_To.length();
	}

	return s_String;
}

uint32_t Utils::StringHash(const std::string& p_String)
{
	uint32_t s_Result = 2166136261U;

	for (auto it = p_String.begin(); it != p_String.end(); ++it)
		s_Result = (16777619 * s_Result) ^ static_cast<unsigned char>(*it);

	return s_Result;
}

std::string Utils::StringTrimRight(const std::string& p_String, char p_Character)
{
	std::string s_String(p_String);

	if (s_String.empty())
		return s_String;

	std::string::iterator s_It;

	for (s_It = s_String.end(); s_It != s_String.begin() && *--s_It == p_Character;);

	if (*s_It != p_Character)
		s_It++;

	s_String.erase(s_It, s_String.end());
	return s_String;
}

std::string Utils::StringTrimLeft(const std::string& p_String, char p_Character)
{
	std::string s_String(p_String);

	if (s_String.empty())
		return s_String;

	std::string::iterator s_It;

	for (s_It = s_String.begin(); s_It != s_String.end() && *++s_It == p_Character;);

	if (*s_It != p_Character)
		s_It--;

	s_String.erase(s_String.begin(), s_It);
	return s_String;
}

std::string Utils::StringTrim(const std::string& p_String, char p_Character)
{
	return StringTrimLeft(StringTrimRight(p_String, p_Character), p_Character);
}

bool Utils::StringStartWith(const std::string& p_String, const std::string& p_Test)
{
	return p_String.find(p_Test) == 0;
}

void Utils::AllocateConsole(const std::string& p_Title)
{
#ifdef _WIN32
	if (!AttachConsole(ATTACH_PARENT_PROCESS))
	{
		AllocConsole();
		AttachConsole(GetCurrentProcessId());
		freopen("CON", "w", stdout);
		SetConsoleTitleA(p_Title.c_str());

		return;
	}
		
	freopen("CONOUT$", "w", stdout);
#endif
}

std::string Utils::GetCurrentDir()
{
	char s_CurrentPath[FILENAME_MAX];

#ifdef _WIN32
	if (!_getcwd(s_CurrentPath, sizeof(s_CurrentPath)))
		return "";
#else
	if (!getcwd(s_CurrentPath, sizeof(s_CurrentPath)))
		return "";
#endif

	s_CurrentPath[sizeof(s_CurrentPath) - 1] = '\0';

	return s_CurrentPath;
}