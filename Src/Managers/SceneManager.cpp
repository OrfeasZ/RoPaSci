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

SceneManager::SceneManager() :
	m_RenderTimer(60)
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

void SceneManager::SetMaxFPS(int p_MaxFPS)
{
	m_RenderTimer.SetUpdatesPerSecond(p_MaxFPS);
}

bool SceneManager::RegisterRenderer(Rendering::IRenderer* p_Renderer)
{
	if (!p_Renderer->Init())
		return false;

	m_Renderers.push_back(p_Renderer);
	return true;
}

void SceneManager::RenderTask(void* p_Argument, int32_t p_ContextID, uint32_t p_Index, uint32_t p_Size)
{
	if (!m_RenderTimer.ShouldUpdate())
		return;

	double s_Delta = m_RenderTimer.GetLastDelta();

	glClearColor(1, 1, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	for (auto s_Renderer : m_Renderers)
		s_Renderer->Render(s_Delta);

	glEnableClientState(GL_VERTEX_ARRAY);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glDisableClientState(GL_VERTEX_ARRAY);

	glfwSwapBuffers(m_Window);
	glfwPollEvents();
}