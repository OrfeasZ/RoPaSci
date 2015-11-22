#pragma once

#include <Misc/StdAfx.h>
#include "IManager.h"

namespace Managers
{
	class SceneManager : public IManager
	{
	
	private:
		SceneManager();
		~SceneManager();

	public:
		bool Init();

	public:

	};
}