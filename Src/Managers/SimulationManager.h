#pragma once

#include <Misc/StdAfx.h>
#include "IManager.h"

#include <Util/EngineTimer.h>

#include <thread>
#include <atomic>

namespace Rendering
{
	class IRenderer;
}

namespace Managers
{
	class SimulationManager : public IManager
	{
	public:
		static SimulationManager* GetInstance();
		static void DestroyInstance();

	private:
		static SimulationManager* m_Instance;

	protected:
		SimulationManager();
		~SimulationManager();

	public:
		virtual bool Init() override;
		void SetTickRate(int p_TickRate);

	protected:
		void Update();

	protected:
		Util::EngineTimer m_SimulationTimer;
	};
}