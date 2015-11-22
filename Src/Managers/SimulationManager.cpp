#include "SimulationManager.h"

#include <Rendering/IRenderer.h>
#include <Managers/InputManager.h>

#include <thread>
#include <chrono>

#include <Game/Main.h>

using namespace Managers;

SimulationManager* SimulationManager::m_Instance = nullptr;

SimulationManager* SimulationManager::GetInstance()
{
	if (!m_Instance)
		m_Instance = new SimulationManager();

	return m_Instance;
}

void SimulationManager::DestroyInstance()
{
	if (!m_Instance)
		return;

	delete m_Instance;
	m_Instance = nullptr;
}

//////////////////////////////////////////////////////////////////////////

SimulationManager::SimulationManager() :
	m_SimulationTimer(128),
	m_InputPostUpdateTask(~0)
{
}

SimulationManager::~SimulationManager()
{
}

bool SimulationManager::Init()
{
	Logger(Util::LogLevel::Info, "Initializing simulation.");

	m_UpdateTask = std::bind(&SimulationManager::UpdateTask, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);

	return true;
}

void SimulationManager::SetTickRate(int p_TickRate)
{
	m_SimulationTimer.SetUpdatesPerSecond(p_TickRate);
}

void SimulationManager::UpdateTask(void* p_Argument, int32_t p_ContextID, uint32_t p_Index, uint32_t p_Size)
{
	if (!m_SimulationTimer.ShouldUpdate())
		return;

	if (m_InputPostUpdateTask != ~0)
	{
		Managers::TaskManager::GetInstance()->WaitForSet(m_InputPostUpdateTask);
		Managers::TaskManager::GetInstance()->ReleaseHandle(m_InputPostUpdateTask);
	}

	auto s_Delta = m_SimulationTimer.GetLastDelta();

	Game::Main::GetInstance()->Update(s_Delta);
	
	// Input PostUpdate Task
	Managers::TaskManager::GetInstance()->CreateTaskSet(
	Managers::InputManager::GetInstance()->GetPostUpdateTask(),
		nullptr,
		1,
		nullptr,
		0,
		"Input_PostUpdate",
		&m_InputPostUpdateTask
	);
}