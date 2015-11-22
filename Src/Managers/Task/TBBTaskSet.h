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

#include <Misc/StdAfx.h>

#include <tbb/task.h>
#include <Util/SpinLock.h>

#include <Managers/TaskManager.h>
#include "GenericTask.h"

namespace Managers
{
	namespace Task
	{
		class TBBTaskSet : public tbb::task
		{
		public:
			TBBTaskSet() :
				m_Function(nullptr), 
				m_Argument(0), 
				m_Size(0), 
				m_SetHandle(-1), 
				m_HasBeenWaitedOn(false)
			{
				m_Name[0] = 0;
				memset(m_Successors, 0, sizeof(m_Successors));
			};

		public:
			task* execute()
			{
				//  set the tbb reference count for this TaskSetTbb to
				//  one plus the task set count
				set_ref_count(m_Size + 1);

				//  Iterate for each task in the set and spawn a GenericTask
				for (uint32_t i = 0; i < m_Size; ++i)
					spawn(*new (allocate_child()) GenericTask(m_Function, m_Argument, i, m_Size, m_Name, m_SetHandle));

				return NULL;
			}

		public:
			TBBTaskSet* m_Successors[5];
			uint32_t m_SetHandle;
			bool m_HasBeenWaitedOn;

			TaskManager::TaskSetFunction_t m_Function;
			void* m_Argument;

			volatile uint32_t m_StartCount;
			volatile uint32_t m_CompletionCount;
			volatile uint32_t m_RefCount;

			uint32_t m_Size;
			Util::SpinLock m_SuccessorsLock;

			char m_Name[512];
		};
	}
}
