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

#include "TaskManager.h"

#include "Task/GenericTask.h"
#include "Task/TBBContextID.h"
#include "Task/TBBTaskSet.h"

using namespace Managers;
using namespace Managers::Task;

TaskManager* TaskManager::m_Instance = nullptr;

TaskManager* TaskManager::GetInstance()
{
	if (!m_Instance)
		m_Instance = new TaskManager();

	return m_Instance;
}

void TaskManager::DestroyInstance()
{
	if (!m_Instance)
		return;

	delete m_Instance;
	m_Instance = nullptr;
}

//////////////////////////////////////////////////////////////////////////

TaskManager::TaskManager() :
	m_TBBContextID(nullptr),
	m_TBBInit(nullptr),
	m_NextFreeSet(0)
{
	memset(m_Sets, 0x00, sizeof(m_Sets));
}

TaskManager::~TaskManager()
{

}

bool TaskManager::Init()
{
	m_TBBContextID = new TBBContextID();
	m_TBBInit = new tbb::task_scheduler_init(tbb::task_scheduler_init::automatic);
	return true;
}

void TaskManager::Shutdown()
{
	for (uint32_t s_Set = 0; s_Set < 255; ++s_Set)
	{
		if (m_Sets[s_Set])
		{
			WaitForSet(s_Set);

			m_Sets[s_Set]->set_ref_count(0);
			m_Sets[s_Set]->destroy(*m_Sets[s_Set]);
			m_Sets[s_Set] = nullptr;
		}
	}

	delete m_TBBInit;
	delete m_TBBContextID;
}


bool TaskManager::CreateTaskSet(TaskSetFunction_t p_Function, void* p_Argument, uint32_t p_TaskCount, uint32_t* p_Depends, uint32_t p_DependCount, char* p_SetName, uint32_t* p_Handle)
{
	uint32_t s_SetHandle;
	uint32_t s_SetParent = (uint32_t) ~0;
	uint32_t* s_Depends = p_Depends;
	uint32_t s_DependCount = p_DependCount;
	bool s_Result = false;

	//  Validate incoming parameters
	if (p_TaskCount == 0 || p_Function == nullptr)
		return false;

	//
	//  Tasksets are spawned when their parents complete.  If no parent for a 
	//  taskset is specified we need to create a fake one.
	//
	if (s_DependCount == 0)
	{
		s_SetParent = AllocateTaskSet();
		m_Sets[s_SetParent]->m_CompletionCount = 0;
		m_Sets[s_SetParent]->m_RefCount = 1;

		//  Implicit starting task never needs to be waited on for TBB since
		//  it is not a real tbb task.
		m_Sets[s_SetParent]->m_HasBeenWaitedOn = true;

		s_DependCount = 1;
		s_Depends = &s_SetParent;
	}

	//
	//  Allocate and setup the internal taskset
	//
	s_SetHandle = AllocateTaskSet();

	m_Sets[s_SetHandle]->m_StartCount = s_DependCount;

	//  NOTE: one refcount is owned by the tasking system the other 
	//  by the caller.
	m_Sets[s_SetHandle]->m_RefCount = 2;

	m_Sets[s_SetHandle]->m_Function = p_Function;
	m_Sets[s_SetHandle]->m_Argument = p_Argument;
	m_Sets[s_SetHandle]->m_Size = p_TaskCount;
	m_Sets[s_SetHandle]->m_CompletionCount = p_TaskCount;
	m_Sets[s_SetHandle]->m_SetHandle = s_SetHandle;

#ifdef PROFILEGPA
	//
	//  Track task name if profiling is enabled
	if (p_SetName)
	{
		StringCbCopyA(
			m_Sets[s_SetHandle]->m_Name,
			sizeof(m_Sets[s_SetHandle]->m_Name),
			p_SetName);
	}
	else
	{
		StringCbCopyA(
			m_Sets[s_SetHandle]->m_Name,
			sizeof(m_Sets[s_SetHandle]->m_Name),
			"Unnamed Task");
	}
#endif // PROFILEGPA

	//
	//  Iterate over the dependency list and setup the successor
	//  pointers in each parent to point to this taskset.
	//
	for (uint32_t i = 0; i < s_DependCount; ++i)
	{
		uint32_t s_DependsOn = s_Depends[i];
		TBBTaskSet* s_DependsOnTaskSet = m_Sets[s_DependsOn];
		uint32_t s_PreviousCompletion;

		//
		//  A taskset with a new successor is consider incomplete even if it
		//  already has completed.  This mechanism allows us tasksets that are
		//  already done to appear active and capable of spawning successors.
		//
		s_PreviousCompletion = ++s_DependsOnTaskSet->m_CompletionCount;

		if (s_PreviousCompletion == 0 && s_SetParent != s_DependsOn)
		{
			//  The dependency taskset was already completed.  This means we have,
			//  or will soon, release the refcount for the tasking system. AddRef
			//  the taskset since the next Completion will release it.
			//  This does not apply to the system-created parent.  
			//
			//  NOTE: There is no race condition here since the caller must still
			//  hold a reference to the dependent taskset which was passed in.
			++s_DependsOnTaskSet->m_RefCount;
		}

		s_DependsOnTaskSet->m_SuccessorsLock.Lock();

		uint32_t s_Successor;

		for (s_Successor = 0; s_Successor < 5; ++s_Successor)
		{
			if (s_DependsOnTaskSet->m_Successors[s_Successor] == nullptr)
			{
				s_DependsOnTaskSet->m_Successors[s_Successor] = m_Sets[s_SetHandle];
				break;
			}
		}

		//
		//  If the successor list is full we have a problem.  The app
		//  needs to give us more space by increasing MAX_SUCCESSORS
		//
		if (s_Successor == 5)
		{
			Logger(Util::LogLevel::Warn, "Too many successors for this task set. Increase MAX_SUCCESSORS.");
			s_DependsOnTaskSet->m_SuccessorsLock.Unlock();
			goto Cleanup;
		}

		s_DependsOnTaskSet->m_SuccessorsLock.Unlock();

		//  
		//  Mark the set as completed for the successor adding operation.
		//
		CompleteTaskSet(s_DependsOn);
	}

	//  Set output taskset handle
	*p_Handle = s_SetHandle;

	s_Result = true;

Cleanup:

	return s_Result;
}


void TaskManager::ReleaseHandle(uint32_t p_SetHandle)
{
	--m_Sets[p_SetHandle]->m_RefCount;

	//
	//  Release cannot destroy the object since TBB may still be
	//  referencing internal members.  Defer destruction until
	//  we need to allocate a slot.    
}


void TaskManager::ReleaseHandles(uint32_t* p_SetHandle, uint32_t p_Count)
{
	for (uint32_t i = 0; i < p_Count; ++i)
		ReleaseHandle(p_SetHandle[i]);
}

void TaskManager::WaitForSet(uint32_t p_SetHandle)
{
	//
	//  Yield the main thread to TBB to get our taskset done faster!
	//  NOTE: tasks can only be waited on once.  After that they will
	//  deadlock if waited on again.
	if (!m_Sets[p_SetHandle]->m_HasBeenWaitedOn)
	{
		m_Sets[p_SetHandle]->wait_for_all();
		m_Sets[p_SetHandle]->m_HasBeenWaitedOn = true;
	}
}

uint32_t TaskManager::AllocateTaskSet()
{
	TBBTaskSet* s_Set = new (tbb::task::allocate_root()) TBBTaskSet();
	uint32_t s_SetHandle = m_NextFreeSet;

	//
	//  Create a new task set and find a slot in the TaskManager to put it in.
	//
	//  NOTE: if we have too many tasks pending we will spin on the slot.  If
	//  spinning occurs, see TaskManager.h and increase MAX_TASKSETS
	//
	s_Set->set_ref_count(2);

	//
	//  NOTE: Allocating tasksets is not thread-safe due to allocation of the
	//  slot for the task pointer.  This can be easily made threadsafe with 
	//  an interlocked op on the muNextFreeSet variable and a spin on the slot.  
	//  It will cost a small amount of performance.
	//
	while (m_Sets[s_SetHandle] != nullptr && m_Sets[s_SetHandle]->m_RefCount != 0)
		s_SetHandle = (s_SetHandle + 1) % 255;

	if (m_Sets[s_SetHandle] != nullptr)
	{
		//  We know the refcount is done, but TBB has an assert that requires
		//  a task be waited on before being deleted.
		WaitForSet(s_SetHandle);

		//
		//  Once TaskManager is done with a tbb object we need to forcibly destroy it.
		//  There are some refcount issues with tasks in tbb 3.0 which can be 
		//  inconsistent if a task has never been waited for.  TaskManager knows the
		//  correct refcount.
		m_Sets[s_SetHandle]->set_ref_count(0);
		m_Sets[s_SetHandle]->destroy(*m_Sets[s_SetHandle]);
		m_Sets[s_SetHandle] = nullptr;
	}

	m_Sets[s_SetHandle] = s_Set;
	m_NextFreeSet = (s_SetHandle + 1) % 255;

	return s_SetHandle;
}

void TaskManager::CompleteTaskSet(uint32_t p_SetHandle)
{
	TBBTaskSet* s_Set = m_Sets[p_SetHandle];
	uint32_t s_CompletionCount = --s_Set->m_CompletionCount;

	if (s_CompletionCount != 0)
		return;

	//
	//  The task set has completed.  We need to look at the successors
	//  and signal them that this dependency of theirs has completed.
	//
	s_Set->m_SuccessorsLock.Lock();

	for (uint32_t i = 0; i < 5; ++i)
	{
		TBBTaskSet* s_Successor = s_Set->m_Successors[i];

		//
		//  A signaled successor must be removed from the Successors list 
		//  before the mSuccessorsLock can be released.
		//
		s_Set->m_Successors[i] = nullptr;

		if (s_Successor != nullptr)
		{
			uint32_t s_StartCount = --s_Successor->m_StartCount;

			//
			//  If the start count is 0 the successor has had all its 
			//  dependencies satisfied and can be scheduled.
			//
			if (s_StartCount == 0)
				s_Successor->execute();
		}
	}

	s_Set->m_SuccessorsLock.Unlock();

	ReleaseHandle(p_SetHandle);
}
