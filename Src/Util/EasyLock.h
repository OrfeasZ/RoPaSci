#pragma once

#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#else
#include <mutex>
#endif

namespace Util
{
	class EasyLock
	{
	public:
		inline EasyLock()
		{
#ifdef _WIN32
			InitializeCriticalSection(&m_Lock);
#endif
		}

		inline ~EasyLock()
		{
#ifdef _WIN32
			DeleteCriticalSection(&m_Lock);
#endif
		}

	public:
		inline void Lock()
		{
#ifdef _WIN32
			EnterCriticalSection(&m_Lock);
#else
			m_Lock.lock();
#endif
		}

		inline void Unlock()
		{
#ifdef _WIN32
			LeaveCriticalSection(&m_Lock);
#else
			m_Lock.unlock();
#endif
		}

	private:
#ifdef _WIN32
		CRITICAL_SECTION m_Lock;
#else
		std::mutex m_Lock;
#endif
	};

	class EasyLockScopeGuard
	{
	public:
		inline EasyLockScopeGuard(EasyLock* p_Lock) :
			m_Lock(p_Lock)
		{
			m_Lock->Lock();
		}

		inline ~EasyLockScopeGuard()
		{
			m_Lock->Unlock();
		}

	private:
		EasyLock* m_Lock;
	};
}