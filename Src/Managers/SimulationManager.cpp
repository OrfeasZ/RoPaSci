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
	m_TickRate(0)
{
}

SimulationManager::~SimulationManager()
{
	m_Renderers.clear();
}

bool SimulationManager::Init()
{
	// TODO: Fix simulation
	Logger(Util::LogLevel::Info, "Initializing simulation at %d tick.", m_TickRate);

	return true;
}

void SimulationManager::SetTickRate(int p_TickRate)
{
	//m_TimingManager.SetUpdatesPerSecond(p_TickRate);
}

void SimulationManager::RegisterRenderer(Rendering::IRenderer* p_Renderer)
{
	m_Renderers.push_back(p_Renderer);
}

void SimulationManager::Update()
{

}