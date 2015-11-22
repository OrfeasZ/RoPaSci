#pragma once

#include <Misc/StdAfx.h>

namespace Managers
{
	class IManager
	{
	public:
		virtual bool Init() = 0;
	};
}