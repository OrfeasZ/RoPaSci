#pragma once

#include <Misc/StdAfx.h>
#include "IManager.h"

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
		void RegisterRenderer(Rendering::IRenderer* p_Renderer);

	protected:
		void Update();

	protected:
		int m_TickRate;

	protected:
		std::vector<Rendering::IRenderer*> m_Renderers;
	};
}