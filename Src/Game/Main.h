#pragma once

#include <Misc/StdAfx.h>

namespace Game
{
	class Main
	{
	public:
		static Main* GetInstance();
		static void DestroyInstance();

	private:
		static Main* m_Instance;

	protected:
		Main();
		~Main();

	public:
		bool Init();
	};
}