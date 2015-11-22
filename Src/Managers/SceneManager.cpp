#include "SceneManager.h"

#include "ShaderManager.h"
#include <Application.h>

#include <Rendering/MainRenderer.h>

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

	if (!RegisterRenderer(Rendering::MainRenderer::GetInstance()))
		return false;

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

void SceneManager::Render()
{
	if (!m_RenderTimer.ShouldUpdate())
		return;

	double s_Delta = m_RenderTimer.GetLastDelta();

	for (auto s_Renderer : m_Renderers)
		s_Renderer->Render(s_Delta);

	glfwSwapBuffers(m_Window);
	glfwPollEvents();
}