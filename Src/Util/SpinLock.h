#pragma once

#include <Misc/StdAfx.h>

namespace Util
{
	class SpinLock
	{
	public:
		inline SpinLock() : 
			m_Lock(0)
		{
		}

	public:
		inline void Lock()
		{
			// TODO: Linux support
			while (_InterlockedCompareExchange(&m_Lock, 1, 0) == 1)
				;
		}

		inline void Unlock()
		{
			m_Lock = 0;
		}

	private:
		volatile uint32_t m_Lock;
	};
}