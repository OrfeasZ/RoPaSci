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

#include "IManager.h"
#include <functional>

namespace Managers
{
	namespace Task
	{
		class TBBTaskSet;
		class GenericTask;
		class TBBContextID;
	}

	class TaskManager : public IManager
	{
	public:
		typedef std::function<void(void*, int32_t, uint32_t, uint32_t)> TaskSetFunction_t;

	public:
		static TaskManager* GetInstance();
		static void DestroyInstance();

	private:
		static TaskManager* m_Instance;

	private:
		TaskManager();
		~TaskManager();

	public:
		bool Init();
		void Shutdown();

		bool CreateTaskSet(TaskSetFunction_t p_Function, void* p_Argument, uint32_t p_TaskCount, uint32_t* p_Depends, uint32_t p_DependCount, char* p_SetName, uint32_t* p_Handle);
		void ReleaseHandle(uint32_t p_SetHandle);
		void ReleaseHandles(uint32_t* p_SetHandle, uint32_t p_Count);
		void WaitForSet(uint32_t p_SetHandle);
		void CompleteTaskSet(uint32_t p_SetHandle);

	private:
		friend class Task::GenericTask;
		uint32_t AllocateTaskSet();

	private:
		Task::TBBTaskSet* m_Sets[255];
		uint32_t m_NextFreeSet;
		Task::TBBContextID* m_TBBContextID;
		void* m_TBBInit;
	};
}