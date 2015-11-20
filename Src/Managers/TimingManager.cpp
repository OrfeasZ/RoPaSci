#include "TimingManager.h"

#include <thread>

using namespace Managers;

TimingManager::TimingManager(int p_UpdatesPerSecond) :
	m_UpdatesPerSecond(p_UpdatesPerSecond),
	m_LastDelta(0.0)
{
	m_LastTick = std::chrono::high_resolution_clock::now().time_since_epoch();
	
	if (m_UpdatesPerSecond <= 0)
		m_UpdatesPerSecond = 1;

	m_TickTime = 1.0 / (double) m_UpdatesPerSecond;
}

TimingManager::~TimingManager()
{

}

void TimingManager::Update()
{
	auto s_CurrentTime = std::chrono::high_resolution_clock::now().time_since_epoch();

	std::chrono::duration<double> s_Delta = s_CurrentTime - m_LastTick;
	m_LastDelta = s_Delta.count();

	if (m_LastDelta <= m_TickTime)
	{
		int s_Microseconds = (int) round((m_TickTime - s_Delta.count()) * 1000000.f);
		std::this_thread::sleep_for(std::chrono::microseconds(s_Microseconds));

		m_LastDelta = m_TickTime;
	}

	m_LastTick = std::chrono::high_resolution_clock::now().time_since_epoch();
}

void TimingManager::SetUpdatesPerSecond(int p_UpdatesPerSecond)
{
	m_UpdatesPerSecond = p_UpdatesPerSecond;

	if (m_UpdatesPerSecond <= 0)
		m_UpdatesPerSecond = 1;

	m_TickTime = 1.0 / (double) m_UpdatesPerSecond;
}