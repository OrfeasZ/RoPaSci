#include "SceneManager.h"

#include <Application.h>

using namespace Managers;

SceneManager* SceneManager::m_Instance = nullptr;

SceneManager* SceneManager::GetInstance()
{
	if (!m_Instance)
		m_Instance = new SceneManager();

	return m_Instance;
}

void SceneManager::DestroyInstance()
{
	if (!m_Instance)
		return;

	delete m_Instance;
	m_Instance = nullptr;
}

//////////////////////////////////////////////////////////////////////////

SceneManager::SceneManager()
{
}

SceneManager::~SceneManager()
{
}

bool SceneManager::Init()
{
	m_Window = Application::GetInstance()->GetWindow();
	m_RenderTask = std::bind(&SceneManager::RenderTask, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
	
	return true;
}

void SceneManager::RenderTask(void* p_Argument, int32_t p_ContextID, uint32_t p_Index, uint32_t p_Size)
{
	glfwSwapBuffers(m_Window);
	glfwPollEvents();
}