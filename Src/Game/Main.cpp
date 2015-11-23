#include "Main.h"

#include <Application.h>

#include <Managers/ModelManager.h>
#include <Managers/InputManager.h>

#include <Game/Entities/GridEntity.h>
#include <Game/Entities/BlockEntity.h>

using namespace Game;

Main* Main::m_Instance = nullptr;

Main* Main::GetInstance()
{
	if (!m_Instance)
		m_Instance = new Main();

	return m_Instance;
}

void Main::DestroyInstance()
{
	if (!m_Instance)
		return;

	delete m_Instance;
	m_Instance = nullptr;
}

Main::Main()
{

}

Main::~Main()
{

}

static Entities::GridEntity* s_Entity;

bool Main::Init()
{
	if (!Managers::ModelManager::GetInstance()->PreCacheModel("GameBlock"))
		return false; 
	
	glfwSetWindowSize(Application::GetInstance()->GetWindow(), 750, 600);

	// TODO: REMOVE THIS TEST CODE
	s_Entity = new Entities::GridEntity(15, 12);
	s_Entity->Init();
	// TEST CODE END

	return true;
}

void Main::Update(double p_Delta)
{
	// User pressed ESC; exit the application.
	if (Managers::InputManager::GetInstance()->IsKeyboardKeyPressed(GLFW_KEY_ESCAPE))
	{
		glfwSetWindowShouldClose(Application::GetInstance()->GetWindow(), GL_TRUE);
		return;
	}

	s_Entity->Update(p_Delta);
}