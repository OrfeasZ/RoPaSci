#pragma once

#include <Misc/StdAfx.h>

#ifndef _WIN32
#include <pthread.h>
#endif

namespace Util
{
	class SpinLock
	{
	public:
		inline SpinLock()
		{
#ifdef _WIN32
     		m_Lock = 0;
#else
			pthread_spin_init(&m_Lock, PTHREAD_PROCESS_SHARED);
#endif
		}

	public:
		inline void Lock()
		{
			// TODO: Linux support
#ifdef _WIN32
			while (_InterlockedCompareExchange(&m_Lock, 1, 0) == 1)
				;
#else
			pthread_spin_lock(&m_Lock);
#endif
		}

		inline void Unlock()
		{
#ifdef _WIN32
			m_Lock = 0;
#else
			pthread_spin_unlock(&m_Lock);
#endif
		}

	private:
#ifdef _WIN32
		volatile uint32_t m_Lock;
#else
		volatile pthread_spinlock_t m_Lock;
#endif
	};
}