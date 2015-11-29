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

#include "TBBContextID.h"

#include <Managers/TaskManager.h>
#include <tbb/task.h>

namespace Managers
{
	namespace Task
	{
		class GenericTask : public tbb::task
		{
		public:
			GenericTask()
				: m_Function(nullptr)
				, m_Argument(0)
				, m_Index(0)
				, m_Size(0)
				, m_SetName((char*) "")
				, m_SetHandle((uint32_t) ~0)
			{
			}

			GenericTask(TaskManager::TaskSetFunction_t p_Function, void* p_Argument, uint32_t p_Index, uint32_t p_Size, char* p_SetName, uint32_t p_SetHandle)
				: m_Function(p_Function)
				, m_Argument(p_Argument)
				, m_Index(p_Index)
				, m_Size(p_Size)
				, m_SetName(p_SetName)
				, m_SetHandle(p_SetHandle)
			{
			}

		public:
			task* execute()
			{
				//ProfileBeginTask(m_SetName);

				m_Function(m_Argument, TBBContextID::g_ContextID.local(), m_Index, m_Size);

				//ProfileEndTask();

				// Notify the TaskManager that this set completed one of its tasks.
				Managers::TaskManager::GetInstance()->CompleteTaskSet(m_SetHandle);

				return nullptr;
			}

		private:
			TaskManager::TaskSetFunction_t m_Function;
			void* m_Argument;
			uint32_t m_Index;
			uint32_t m_Size;
			char* m_SetName;
			uint32_t m_SetHandle;
		};
	}
}
