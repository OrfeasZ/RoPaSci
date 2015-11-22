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
	m_TickRate(0),
	m_Running(false)
{
}

SimulationManager::~SimulationManager()
{
	Stop();
	m_Renderers.clear();
}

bool SimulationManager::Init()
{
	return true;

	// TODO: Fix simulation
	if (m_Running)
		return false;

	Logger(Util::LogLevel::Info, "Initializing simulation at %d tick.", m_TickRate);

	m_Running = true;
	m_WorkerThread = std::thread(&SimulationManager::Update, this);

	return true;
}

void SimulationManager::Stop()
{
	m_Running = false;
	m_WorkerThread.join();
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
	while (m_Running)
	{
		//m_TimingManager.Update();
		//double s_Delta = m_TimingManager.GetLastDelta();

		//for (auto s_Renderer : m_Renderers)
		//	s_Renderer->Update(s_Delta);
	}
}