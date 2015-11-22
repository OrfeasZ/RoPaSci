//////////////////////////////////////////////////////////////////////////
// Code adapted from the Intel "Game Engine Tasking Animation" Demo 
// https://software.intel.com/en-us/articles/game-engine-tasking-animation
//////////////////////////////////////////////////////////////////////////

/*
    Copyright 2010 Intel Corporation
	All Rights Reserved

	Permission is granted to use, copy, distribute and prepare derivative works of this
	software for any purpose and without fee, provided, that the above copyright notice
	and this statement appear in all copies.  Intel makes no representations about the
	suitability of this software for any purpose.  THIS SOFTWARE IS PROVIDED ""AS IS.""
	INTEL SPECIFICALLY DISCLAIMS ALL WARRANTIES, EXPRESS OR IMPLIED, AND ALL LIABILITY,
	INCLUDING CONSEQUENTIAL AND OTHER INDIRECT DAMAGES, FOR THE USE OF THIS SOFTWARE,
	INCLUDING LIABILITY FOR INFRINGEMENT OF ANY PROPRIETARY RIGHTS, AND INCLUDING THE
	WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  Intel does not
	assume any responsibility for any errors which may appear in this software nor any
	responsibility to update it.
*/

#pragma once

#include <tbb/atomic.h>
#include <tbb/task_scheduler_observer.h>
#include <tbb/enumerable_thread_specific.h>

namespace Managers
{
	namespace Task
	{
		class TBBContextID : public tbb::task_scheduler_observer
		{
		public:
			static tbb::atomic<int32_t> g_ContextIDCount;
			static tbb::enumerable_thread_specific<int32_t> g_ContextID;

		public:
			TBBContextID()
			{
				g_ContextIDCount = 0;
				observe(true);
			}

		public:
			virtual void on_scheduler_entry(bool p_IsWorker) override
			{
				int32_t s_Context = g_ContextIDCount.fetch_and_increment();
				g_ContextID.local() = s_Context;
			}
		};
	}
}