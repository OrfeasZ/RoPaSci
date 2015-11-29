#include "Main.h"

#include <Application.h>

#include <Managers/ModelManager.h>
#include <Managers/InputManager.h>
#include <Managers/TextureManager.h>

#include <Rendering/UIRenderer.h>

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

void Main::Render(double p_Delta)
{
	// Render the UI.
	// Main UI Overlay.
	float s_Size = Application::GetInstance()->WindowHeight() * 1.146153846153846;

	Rendering::UIRenderer::GetInstance()->RenderTexture(
		(Rendering::Textures::Texture*) Managers::TextureManager::GetInstance()->GetTexture("main_ui"),
		(Application::GetInstance()->WindowHeight() - s_Size) / 2,
		(Application::GetInstance()->WindowWidth() - s_Size) / 2,
		((Application::GetInstance()->WindowHeight() - s_Size) / 2) + s_Size,
		((Application::GetInstance()->WindowWidth() - s_Size) / 2) + s_Size
	);

	// Borders. Note: VERY HACKY
	if ((Application::GetInstance()->WindowWidth() - s_Size) / 2 > 0)
	{
		Rendering::UIRenderer::GetInstance()->RenderTexture(
			(Rendering::Textures::Texture*) Managers::TextureManager::GetInstance()->GetTexture("ui_background"),
			0,
			0,
			Application::GetInstance()->WindowHeight(),
			(Application::GetInstance()->WindowWidth() - s_Size) / 2
		);

		Rendering::UIRenderer::GetInstance()->RenderTexture(
			(Rendering::Textures::Texture*) Managers::TextureManager::GetInstance()->GetTexture("ui_background"),
			0,
			((Application::GetInstance()->WindowWidth() - s_Size) / 2) + s_Size,
			Application::GetInstance()->WindowHeight(),
			Application::GetInstance()->WindowWidth()
		);
	}

	float s_ScoreLeft = ((Application::GetInstance()->WindowWidth() - s_Size) / 2) + (s_Size * 0.13);
	float s_MovesLeft = ((Application::GetInstance()->WindowWidth() - s_Size) / 2) + (s_Size * 0.88);

	float s_TextTop = ((Application::GetInstance()->WindowHeight() - s_Size) / 2) + (s_Size * 0.914);

	float s_TextSize = ((s_Size - 250.f) / 350.f) / 10.f;

	Rendering::UIRenderer::GetInstance()->RenderText("999999", s_ScoreLeft, s_TextTop, s_TextSize, glm::vec3(1.0, 1.0, 1.0));
	Rendering::UIRenderer::GetInstance()->RenderText("999", s_MovesLeft, s_TextTop, s_TextSize, glm::vec3(1.0, 1.0, 1.0));
}