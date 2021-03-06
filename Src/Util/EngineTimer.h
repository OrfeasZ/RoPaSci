#pragma once

#include <Misc/StdAfx.h>
#include <chrono>
#include <atomic>

namespace Util
{
	class EngineTimer
	{
	public:
		EngineTimer(int p_UpdatesPerSecond);
		~EngineTimer();

	public:
		void UpdateLock();
		bool ShouldUpdate();
		void SetUpdatesPerSecond(int p_UpdatesPerSecond);

	public:
		inline double GetLastDelta() const { return m_LastDelta; }

	protected:
		double m_TickTime;
		double m_LastDelta;
		int m_UpdatesPerSecond;
		std::chrono::duration<std::chrono::system_clock::rep, std::chrono::system_clock::period> m_LastTick;
	};
}