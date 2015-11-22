#include "SimulationManager.h"

#include <Rendering/IRenderer.h>

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
	m_SimulationTimer(128)
{
}

SimulationManager::~SimulationManager()
{
}

bool SimulationManager::Init()
{
	// TODO: Fix simulation
	Logger(Util::LogLevel::Info, "Initializing simulation.");

	return true;
}

void SimulationManager::SetTickRate(int p_TickRate)
{
	m_SimulationTimer.SetUpdatesPerSecond(p_TickRate);
}

void SimulationManager::Update()
{
	if (!m_SimulationTimer.ShouldUpdate())
		return;

	auto s_Delta = m_SimulationTimer.GetLastDelta();
}