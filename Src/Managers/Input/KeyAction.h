#pragma once

#include <Misc/StdAfx.h>

namespace Managers
{
	namespace Input
	{
		struct KeyAction
		{
			int ScanCode;
			int Action;
			int Modifiers;
		};
	}
}