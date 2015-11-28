#include "BlockEntity.h"

#include <Managers/ModelManager.h>
#include <Managers/ShaderManager.h>
#include <Managers/TextureManager.h>

using namespace Game::Entities;

BlockEntity::BlockEntity(BlockType p_Type, int p_X, int p_Y, int p_Columns, int p_Rows) :
	m_Type(p_Type),
	m_X(p_X), 
	m_Y(p_Y),
	m_GridColumns(p_Columns),
	m_GridRows(p_Rows),
	m_Model(nullptr),
	m_Active(false),
	m_Hover(false),
	m_TargetX(0.f),
	m_TargetY(0.f),
	m_TargetZ(0.f),
	m_AnimatingMovement(false),
	m_ResetHeight(false),
	m_Destroyed(false),
	m_PendingPosition(false)
{
	Logger(Util::LogLevel::Debug, "Creating BlockEntity of type '%d' at %dx%d.", m_Type, m_X, m_Y);
}

BlockEntity::~BlockEntity()
{
	if (m_Model)
		Managers::ModelManager::GetInstance()->RemoveModel(m_Model);
}

void BlockEntity::Init()
{
	m_Model = new Rendering::Objects::Model(
		Managers::ModelManager::GetInstance()->GetModelData("GameBlock"),
		Managers::ShaderManager::GetInstance()->GetShaderProgram("ShadedModel")
	);

	switch (m_Type)
	{
	case BlockEntity::Rock:
		m_Model->Color(glm::vec3(0.68, 0.4, 0.19));
		m_Model->ShaderProgram(Managers::ShaderManager::GetInstance()->GetShaderProgram("TexturedModel"));
		m_Model->Texture(Managers::TextureManager::GetInstance()->GetTexture("rock"));
		break;

	case BlockEntity::Paper:
		m_Model->Color(glm::vec3(1.0, 1.0, 1.0));
		m_Model->ShaderProgram(Managers::ShaderManager::GetInstance()->GetShaderProgram("TexturedModel"));
		m_Model->Texture(Managers::TextureManager::GetInstance()->GetTexture("paper"));
		break;

	case BlockEntity::Scissors:
		m_Model->Color(glm::vec3(0.12, 0.82, 0.18));
		m_Model->ShaderProgram(Managers::ShaderManager::GetInstance()->GetShaderProgram("TexturedModel"));
		m_Model->Texture(Managers::TextureManager::GetInstance()->GetTexture("scissors"));
		break;

	case BlockEntity::Red:
		m_Model->Color(glm::vec3(0.92, 0.16, 0.16));
		m_Model->ShaderProgram(Managers::ShaderManager::GetInstance()->GetShaderProgram("ShadedModel"));
		m_Model->Texture(nullptr);
		break;

	case BlockEntity::Blue:
		m_Model->Color(glm::vec3(0.16, 0.52, 0.91));
		m_Model->ShaderProgram(Managers::ShaderManager::GetInstance()->GetShaderProgram("ShadedModel"));
		m_Model->Texture(nullptr);
		break;

	case BlockEntity::Bomb:
		m_Model->Color(glm::vec3(1.0, 1.0, 1.0));
		m_Model->ShaderProgram(Managers::ShaderManager::GetInstance()->GetShaderProgram("TexturedModel"));
		m_Model->Texture(Managers::TextureManager::GetInstance()->GetTexture("bomb"));
		break;
	}

	float s_CenterX = (m_GridColumns - 1.0) / 2.0;
	float s_CenterY = (m_GridRows - 1.0) / 2.0;

	m_Model->Translate(glm::vec3((m_X - s_CenterX) * 0.30, 0.0, (m_Y - s_CenterY) * 0.30));

	m_TargetX = (m_X - s_CenterX) * 0.30;
	m_TargetY = 0.0;
	m_TargetZ = (m_Y - s_CenterY) * 0.30;

	Managers::ModelManager::GetInstance()->RegisterModel(m_Model);
}

void BlockEntity::Update(double p_Delta)
{
	if (m_ScaleStep > 0)
	{
		m_Model->Scale(glm::vec3(1.0 - (m_ScaleStep * p_Delta), 1.0 - (m_ScaleStep * p_Delta), 1.0 - (m_ScaleStep * p_Delta)));
		m_ScaleTimer += p_Delta;

		if (m_ScaleTimer >= 1.f / m_ScaleStep)
		{
			m_ScaleStep = 0.f;
			m_Model->SetScale(glm::vec3(1.f, 1.f, 1.f));
		}
	}

	if (m_AnimatingMovement)
	{
		if (m_MovementDelay > 0.f)
		{
			m_MovementDelay -= p_Delta;

			if (m_MovementDelay < 0.f)
				m_MovementDelay = 0.f;
		}
		else
		{
			auto s_Position = m_Model->Position();

			glm::vec3 s_Animation(0.f, 0.f, 0.f);

			bool s_Animating = false;

			if (s_Position.x < m_TargetX)
			{
				s_Animating = true;
				s_Animation.x += 1.5 * p_Delta;

				if (s_Position.x + s_Animation.x > m_TargetX)
					s_Animation.x = m_TargetX - s_Position.x;
			}

			if (s_Position.x > m_TargetX)
			{
				s_Animating = true;
				s_Animation.x -= 1.5 * p_Delta;

				if (s_Position.x - s_Animation.x < m_TargetX)
					s_Animation.x = m_TargetX - s_Position.x;
			}

			if (s_Position.y < m_TargetY)
			{
				s_Animating = true;
				s_Animation.y += 1.5 * p_Delta;

				if (s_Position.y + s_Animation.y > m_TargetY)
					s_Animation.y = m_TargetY - s_Position.y;
			}

			if (s_Position.y > m_TargetY)
			{
				s_Animating = true;
				s_Animation.y -= 1.5 * p_Delta;

				if (s_Position.y - s_Animation.y < m_TargetY)
					s_Animation.y = m_TargetY - s_Position.y;
			}

			if (s_Position.z < m_TargetZ)
			{
				s_Animating = true;
				s_Animation.z += 1.5 * p_Delta;

				if (s_Position.z + s_Animation.z > m_TargetZ)
					s_Animation.z = m_TargetZ - s_Position.z;
			}

			if (s_Position.z > m_TargetZ)
			{
				s_Animating = true;
				s_Animation.z -= 1.5 * p_Delta;

				if (s_Position.z - s_Animation.z < m_TargetZ)
					s_Animation.z = m_TargetZ - s_Position.z;
			}

			if (!s_Animating)
				m_AnimatingMovement = false;

			m_Model->Translate(s_Animation);
		}
	}

	if (!m_AnimatingMovement && m_ResetHeight)
	{
		m_TargetY = 0.f;
		m_ResetHeight = false;
		m_AnimatingMovement = true;
	}

	if (m_PendingPosition)
	{
		if (m_MovementDelay > 0.f)
		{
			m_MovementDelay -= p_Delta;

			if (m_MovementDelay < 0.f)
				m_MovementDelay = 0.f;
		}
		else
		{
			m_PendingPosition = false;

			float s_CenterX = (m_GridColumns - 1.0) / 2.0;
			float s_CenterY = (m_GridRows - 1.0) / 2.0;

			float s_TargetX = (m_PendingX - s_CenterX) * 0.30;
			float s_TargetZ = (m_PendingY - s_CenterY) * 0.30;

			m_Model->Position(glm::vec3(s_TargetX, m_TargetY, s_TargetZ));
			m_Model->SetScale(glm::vec3(1.f, 1.f, 1.f));
			m_AnimatingMovement = true;
		}
	}
}

void BlockEntity::Type(BlockType p_Type)
{
	if (m_Type == p_Type)
		return;

	m_Type = p_Type;

	Managers::ModelManager::GetInstance()->RemoveModel(m_Model);

	switch (m_Type)
	{
	case BlockEntity::Rock:
		m_Model->Color(glm::vec3(0.68, 0.4, 0.19));
		m_Model->ShaderProgram(Managers::ShaderManager::GetInstance()->GetShaderProgram("TexturedModel"));
		m_Model->Texture(Managers::TextureManager::GetInstance()->GetTexture("rock"));
		break;

	case BlockEntity::Paper:
		m_Model->Color(glm::vec3(1.0, 1.0, 1.0));
		m_Model->ShaderProgram(Managers::ShaderManager::GetInstance()->GetShaderProgram("TexturedModel"));
		m_Model->Texture(Managers::TextureManager::GetInstance()->GetTexture("paper"));
		break;

	case BlockEntity::Scissors:
		m_Model->Color(glm::vec3(0.12, 0.82, 0.18));
		m_Model->ShaderProgram(Managers::ShaderManager::GetInstance()->GetShaderProgram("TexturedModel"));
		m_Model->Texture(Managers::TextureManager::GetInstance()->GetTexture("scissors"));
		break;

	case BlockEntity::Red:
		m_Model->Color(glm::vec3(0.92, 0.16, 0.16));
		m_Model->ShaderProgram(Managers::ShaderManager::GetInstance()->GetShaderProgram("ShadedModel"));
		m_Model->Texture(nullptr);
		break;

	case BlockEntity::Blue:
		m_Model->Color(glm::vec3(0.16, 0.52, 0.91));
		m_Model->ShaderProgram(Managers::ShaderManager::GetInstance()->GetShaderProgram("ShadedModel"));
		m_Model->Texture(nullptr);
		break;

	case BlockEntity::Bomb:
		m_Model->Color(glm::vec3(1.0, 1.0, 1.0));
		m_Model->ShaderProgram(Managers::ShaderManager::GetInstance()->GetShaderProgram("TexturedModel"));
		m_Model->Texture(Managers::TextureManager::GetInstance()->GetTexture("bomb"));
		break;
	}

	Managers::ModelManager::GetInstance()->RegisterModel(m_Model);
}

void BlockEntity::Position(int p_X, int p_Y, bool p_Simulated, float p_Delay)
{
	if (m_X == p_X && m_Y == p_Y)
		return;

	auto s_TargetX = m_TargetX;
	auto s_TargetZ = m_TargetZ;

	m_Y = p_Y;
	m_X = p_X;

	float s_CenterX = (m_GridColumns - 1.0) / 2.0;
	float s_CenterY = (m_GridRows - 1.0) / 2.0;

	m_TargetX = (p_X - s_CenterX) * 0.30;
	m_TargetZ = (p_Y - s_CenterY) * 0.30;

	m_MovementDelay = p_Delay;

	if (!p_Simulated)
	{
		m_AnimatingMovement = true;
		m_ResetHeight = true;
		m_Active = false;
		
		return;
	}

	m_PendingX = p_X;
	m_PendingY = p_Y;
	m_PendingPosition = true;
}

void BlockEntity::Active(bool p_Active)
{
	if (m_Active == p_Active)
		return;

	m_Active = p_Active;

	if (m_Active)
		m_TargetY = 0.15f;
	else
		m_TargetY = 0.f;

	m_AnimatingMovement = true;
}

void BlockEntity::Hover(bool p_Hover)
{

}

void BlockEntity::Destroyed(bool p_Destroyed)
{
	// TODO: Animation
	m_Destroyed = p_Destroyed;
	m_AnimatingMovement = false;
	m_Active = false;

	if (m_Destroyed)
	{
		m_ScaleTimer = 0.f;
		m_ScaleStep = 2.0f;
	}
}

void BlockEntity::MoveToTop(int p_Y, float p_Delay)
{
	m_PendingX = m_X;
	m_PendingY = p_Y;
	m_MovementDelay = p_Delay;

	m_PendingPosition = true;
	m_AnimatingMovement = false;

	return;
}