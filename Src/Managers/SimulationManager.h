#pragma once

#include <Misc/StdAfx.h>
#include "IManager.h"

#include <Util/EngineTimer.h>
#include <Managers/TaskManager.h>

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

	public:
		inline TaskManager::TaskSetFunction_t GetUpdateTask() const { return m_UpdateTask; }

	protected:
		void UpdateTask(void* p_Argument, int32_t p_ContextID, uint32_t p_Index, uint32_t p_Size);

	protected:
		Util::EngineTimer m_SimulationTimer;

	protected:
		// Tasks
		TaskManager::TaskSetFunction_t m_UpdateTask;
		uint32_t m_InputPostUpdateTask;
	};
}