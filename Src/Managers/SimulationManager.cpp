#include "SimulationManager.h"

#include <Rendering/IRenderer.h>

using namespace Managers;

SimulationManager::SimulationManager(int p_TickRate) :
	m_TickRate(p_TickRate),
	m_TimingManager(p_TickRate),
	m_Running(false)
{
}

SimulationManager::~SimulationManager()
{
	Stop();
	m_Renderers.clear();
}

void SimulationManager::Init()
{
	if (m_Running)
		return;

	Logger(Util::LogLevel::Info, "Initializing simulation at %d tick.", m_TickRate);

	m_Running = true;
	m_WorkerThread = std::thread(&SimulationManager::Update, this);
}

void SimulationManager::Stop()
{
	m_Running = false;
	m_WorkerThread.join();
}

void SimulationManager::SetTickRate(int p_TickRate)
{
	m_TimingManager.SetUpdatesPerSecond(p_TickRate);
}

void SimulationManager::RegisterRenderer(Rendering::IRenderer* p_Renderer)
{
	m_Renderers.push_back(p_Renderer);
}

void SimulationManager::Update()
{
	while (m_Running)
	{
		m_TimingManager.Update();
		double s_Delta = m_TimingManager.GetLastDelta();

		for (auto s_Renderer : m_Renderers)
			s_Renderer->Update(s_Delta);
	}
}