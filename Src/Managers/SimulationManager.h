#pragma once

#include <Misc/StdAfx.h>

#include <thread>
#include <atomic>

#include "TimingManager.h"

namespace Rendering
{
	class IRenderer;
}

namespace Managers
{
	class SimulationManager
	{
	public:
		SimulationManager(int p_TickRate);
		~SimulationManager();

	public:
		void Init();
		void Stop();

		void SetTickRate(int p_TickRate);
		void RegisterRenderer(Rendering::IRenderer* p_Renderer);

	protected:
		void Update();

	protected:
		int m_TickRate;
		volatile bool m_Running;
		std::thread m_WorkerThread;
		TimingManager m_TimingManager;

	protected:
		std::vector<Rendering::IRenderer*> m_Renderers;
	};
}