#include "Main.h"

#include <Application.h>

#include <Managers/ModelManager.h>
#include <Managers/InputManager.h>
#include <Managers/TextureManager.h>

#include <Rendering/UIRenderer.h>

#include <Game/Entities/GridEntity.h>
#include <Game/Entities/BlockEntity.h>
#include <Game/Entities/ScoringEntity.h>
#include <Game/Entities/LifeEntity.h>

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

Main::Main() :
	m_GridEntity(nullptr),
	m_State(Main::IdleStart),
	m_SelectedMoves(0.f)
{

}

Main::~Main()
{
	if (m_GridEntity)
		delete m_GridEntity;

	m_GridEntity = nullptr;
}

bool Main::Init()
{
	if (!Managers::ModelManager::GetInstance()->PreCacheModel("GameBlock"))
		return false; 

	m_State = Main::IdleStart;
	m_GridEntity = nullptr;
	m_SelectedMoves = 10.f;

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

	switch (m_State)
	{
		case Main::IdleStart:
		{
			// Start the game if B is pressed.
			if (Managers::InputManager::GetInstance()->IsKeyboardKeyPressed(GLFW_KEY_B))
			{
				if (m_GridEntity)
					delete m_GridEntity;

				m_GridEntity = new Entities::GridEntity(15, 12, (uint32_t) round(m_SelectedMoves));
				m_GridEntity->Init();

				m_State = Main::Playing;
				break;
			}

			// Increment or decrement move count based on input.
			if (Managers::InputManager::GetInstance()->IsKeyboardKeyPressed(GLFW_KEY_LEFT) && m_SelectedMoves > 1.f)
				m_SelectedMoves -= 0.05f;

			if (Managers::InputManager::GetInstance()->IsKeyboardKeyPressed(GLFW_KEY_RIGHT) && m_SelectedMoves < 99.f)
				m_SelectedMoves += 0.05f;

			break;
		}

		case Main::Playing:
		{
			m_GridEntity->Update(p_Delta);

			// Check if we've lost the game.
			if (!m_GridEntity->Processing() && m_GridEntity->GetLifeEntity() && m_GridEntity->GetLifeEntity()->GetLives() <= 0)
				m_State = Main::GameOver;

			break;
		}

		case Main::GameOver:
		{
			if (Managers::InputManager::GetInstance()->IsKeyboardKeyPressed(GLFW_KEY_B))
			{
				if (m_GridEntity)
					delete m_GridEntity;

				m_GridEntity = new Entities::GridEntity(15, 12, (uint32_t) round(m_SelectedMoves));
				m_GridEntity->Init();

				m_State = Main::Playing;
			}

			break;
		}
	}
}

void Main::Render(double p_Delta)
{
	// Render the UI.
	// Main UI Overlay.
	float s_Size = Application::GetInstance()->WindowHeight() * 1.1461538f;

	Rendering::Textures::Texture* s_MainTexture = nullptr;

	switch (m_State)
	{
	case Main::IdleStart:
		s_MainTexture = (Rendering::Textures::Texture*) Managers::TextureManager::GetInstance()->GetTexture("main_ui_idle");
		break;

	case Main::Playing:
		s_MainTexture = (Rendering::Textures::Texture*) Managers::TextureManager::GetInstance()->GetTexture("main_ui");
		break;

	case Main::GameOver:
		s_MainTexture = (Rendering::Textures::Texture*) Managers::TextureManager::GetInstance()->GetTexture("main_ui_gameover");
		break;
	}

	Rendering::UIRenderer::GetInstance()->RenderTexture(
		s_MainTexture,
		(Application::GetInstance()->WindowHeight() - s_Size) / 2.f,
		(Application::GetInstance()->WindowWidth() - s_Size) / 2.f,
		((Application::GetInstance()->WindowHeight() - s_Size) / 2.f) + s_Size,
		((Application::GetInstance()->WindowWidth() - s_Size) / 2.f) + s_Size
	);

	// Borders. Note: VERY HACKY
	if ((Application::GetInstance()->WindowWidth() - s_Size) / 2.f > 0.f)
	{
		Rendering::UIRenderer::GetInstance()->RenderTexture(
			(Rendering::Textures::Texture*) Managers::TextureManager::GetInstance()->GetTexture("ui_background"),
			0.f,
			0.f,
			(float) Application::GetInstance()->WindowHeight(),
			(Application::GetInstance()->WindowWidth() - s_Size) / 2.f
		);

		Rendering::UIRenderer::GetInstance()->RenderTexture(
			(Rendering::Textures::Texture*) Managers::TextureManager::GetInstance()->GetTexture("ui_background"),
			0.f,
			((Application::GetInstance()->WindowWidth() - s_Size) / 2.f) + s_Size,
			(float) Application::GetInstance()->WindowHeight(),
			(float) Application::GetInstance()->WindowWidth()
		);
	}

	// Score and lives text.
	float s_ScoreLeftOffset = ((Application::GetInstance()->WindowWidth() - s_Size) / 2.f) + (s_Size * 0.13f);
	float s_LivesLeftOffset = ((Application::GetInstance()->WindowWidth() - s_Size) / 2.f) + (s_Size * 0.88f);

	float s_TextTopOffset = ((Application::GetInstance()->WindowHeight() - s_Size) / 2.f) + (s_Size * 0.914f);

	float s_TextSize = ((s_Size - 250.f) / 350.f) / 10.f;

	char s_Score[128];
	sprintf(s_Score, "%d", m_GridEntity ? (m_GridEntity->GetScoringEntity() ? m_GridEntity->GetScoringEntity()->GetScore() : 0) : 0);

	char s_Lives[128];
	sprintf(s_Lives, "%d", m_GridEntity ? (m_GridEntity->GetLifeEntity() ? m_GridEntity->GetLifeEntity()->GetLives() : 0) : 0);

	Rendering::UIRenderer::GetInstance()->RenderText(s_Score, s_ScoreLeftOffset, s_TextTopOffset, s_TextSize, glm::vec3(1.0, 1.0, 1.0));
	Rendering::UIRenderer::GetInstance()->RenderText(s_Lives, s_LivesLeftOffset, s_TextTopOffset, s_TextSize, glm::vec3(1.0, 1.0, 1.0));

	// Moves text.
	if (m_State == Main::IdleStart)
	{
		uint32_t s_MovesNumber = (uint32_t) round(m_SelectedMoves);

		float s_MovesLeftPercentage = s_MovesNumber < 10 ? 0.58f : 0.57f;

		float s_MovesLeftOffset = ((Application::GetInstance()->WindowWidth() - s_Size) / 2.f) + (s_Size * s_MovesLeftPercentage);
		float s_MovesTopOffset = ((Application::GetInstance()->WindowHeight() - s_Size) / 2.f) + (s_Size * 0.445f);

		char s_Moves[128];
		sprintf(s_Moves, "%d", s_MovesNumber);

		Rendering::UIRenderer::GetInstance()->RenderText(s_Moves, s_MovesLeftOffset, s_MovesTopOffset, s_TextSize, glm::vec3(1.0, 1.0, 1.0));
	}
}