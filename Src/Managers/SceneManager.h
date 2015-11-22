#pragma once

#include <Misc/StdAfx.h>
#include "IManager.h"

namespace Managers
{
	class SceneManager : public IManager
	{
	public:
		static SceneManager* GetInstance();
		static void DestroyInstance();

	private:
		static SceneManager* m_Instance;

	private:
		SceneManager();
		~SceneManager();

	public:
		bool Init();

	public:

	};
}