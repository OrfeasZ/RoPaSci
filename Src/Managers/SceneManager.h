#pragma once

#include <Misc/StdAfx.h>

#include "IManager.h"
#include "TaskManager.h"

namespace Managers
{
	class SceneManager : public IManager
	{
	public:
		static SceneManager* GetInstance();
		static void DestroyInstance();

	private:
		static SceneManager* m_Instance;

	private:
		SceneManager();
		~SceneManager();

	public:
		virtual bool Init() override;

	public:
		// Tasks
		void RenderTask(void* p_Argument, int32_t p_ContextID, uint32_t p_Index, uint32_t p_Size);
		inline TaskManager::TaskSetFunction_t GetRenderTask() const { return m_RenderTask; }

	protected:
		GLFWwindow* m_Window;

	protected:
		// Tasks
		TaskManager::TaskSetFunction_t m_RenderTask;
	};
}