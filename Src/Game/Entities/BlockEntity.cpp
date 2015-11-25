#include "BlockEntity.h"

#include <Managers/ModelManager.h>
#include <Managers/ShaderManager.h>

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
	m_Animating(false),
	m_ResetHeight(false),
	m_Destroyed(false)
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
		m_Model->Color(glm::vec3(1.0, 1.0, 0.0));
		break;

	case BlockEntity::Paper:
		m_Model->Color(glm::vec3(0.0, 1.0, 1.0));
		break;

	case BlockEntity::Scissors:
		m_Model->Color(glm::vec3(0.0, 1.0, 0.0));
		break;

	case BlockEntity::Red:
		m_Model->Color(glm::vec3(1.0, 0.0, 0.0));
		break;

	case BlockEntity::Blue:
		m_Model->Color(glm::vec3(0.0, 0.0, 1.0));
		break;

	case BlockEntity::Bomb:
		m_Model->Color(glm::vec3(0.25, 0.25, 0.25));
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
	if (m_Animating)
	{
		auto s_Position = m_Model->Position();

		glm::vec3 s_Animation(0.f, 0.f, 0.f);

		bool s_Animating = false;

		if (s_Position.x < m_TargetX)
		{
			s_Animating = true;
			s_Animation.x += 0.5 * p_Delta;

			if (s_Position.x + s_Animation.x > m_TargetX)
				s_Animation.x = m_TargetX - s_Position.x;
		}

		if (s_Position.x > m_TargetX)
		{
			s_Animating = true;
			s_Animation.x -= 0.5 * p_Delta;

			if (s_Position.x - s_Animation.x < m_TargetX)
				s_Animation.x = m_TargetX - s_Position.x;
		}

		if (s_Position.y < m_TargetY)
		{
			s_Animating = true;
			s_Animation.y += 0.5 * p_Delta;

			if (s_Position.y + s_Animation.y > m_TargetY)
				s_Animation.y = m_TargetY - s_Position.y;
		}

		if (s_Position.y > m_TargetY)
		{
			s_Animating = true;
			s_Animation.y -= 0.5 * p_Delta;

			if (s_Position.y - s_Animation.y < m_TargetY)
				s_Animation.y = m_TargetY - s_Position.y;
		}

		if (s_Position.z < m_TargetZ)
		{
			s_Animating = true;
			s_Animation.z += 0.5 * p_Delta;

			if (s_Position.z + s_Animation.z > m_TargetZ)
				s_Animation.z = m_TargetZ - s_Position.z;
		}

		if (s_Position.z > m_TargetZ)
		{
			s_Animating = true;
			s_Animation.z -= 0.5 * p_Delta;

			if (s_Position.z - s_Animation.z < m_TargetZ)
				s_Animation.z = m_TargetZ - s_Position.z;
		}

		if (!s_Animating)
			m_Animating = false;

		m_Model->Translate(s_Animation);
	}

	if (!m_Animating && m_ResetHeight)
	{
		m_TargetY = 0.f;
		m_ResetHeight = false;
		m_Animating = true;
	}
}

void BlockEntity::Type(BlockType p_Type)
{
	if (m_Type == p_Type)
		return;

	m_Type = p_Type;

	switch (m_Type)
	{
	case BlockEntity::Rock:
		m_Model->Color(glm::vec3(1.0, 1.0, 0.0));
		break;

	case BlockEntity::Paper:
		m_Model->Color(glm::vec3(0.0, 1.0, 1.0));
		break;

	case BlockEntity::Scissors:
		m_Model->Color(glm::vec3(0.0, 1.0, 0.0));
		break;

	case BlockEntity::Red:
		m_Model->Color(glm::vec3(1.0, 0.0, 0.0));
		break;

	case BlockEntity::Blue:
		m_Model->Color(glm::vec3(0.0, 0.0, 1.0));
		break;

	case BlockEntity::Bomb:
		m_Model->Color(glm::vec3(0.15, 0.15, 0.15));
		break;
	}
}

void BlockEntity::Position(int p_X, int p_Y, bool p_Simulated)
{
	if (m_X == p_X && m_Y == p_Y)
		return;

	auto s_TargetX = m_TargetX;
	auto s_TargetZ = m_TargetZ;

	m_Y = p_Y;
	m_X = p_X;

	float s_CenterX = (m_GridColumns - 1.0) / 2.0;
	float s_CenterY = (m_GridRows - 1.0) / 2.0;

	m_TargetX = (m_X - s_CenterX) * 0.30;
	m_TargetZ = (m_Y - s_CenterY) * 0.30;

	if (!p_Simulated)
	{
		m_Animating = true;
		m_ResetHeight = true;
		m_Active = false;
		
		return;
	}

	m_Model->Position(glm::vec3(m_TargetX, m_TargetY, m_TargetZ));
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

	m_Animating = true;
}

void BlockEntity::Hover(bool p_Hover)
{
	/*if (m_Hover == p_Hover)
		return;

	m_Hover = p_Hover;

	if (m_Active)
		return;

	if (m_Hover)
	{
		m_Model->Translate(glm::vec3(0.0, 0.15, 0.0));
	}
	else
	{
		m_Model->Translate(glm::vec3(0.0, -0.15, 0.0));
	}

	return;

	if (m_Hover)
	{
		m_Model->Color(glm::vec3(0.5, 0.5, 1.0));
		return;
	}

	switch (m_Type)
	{
	case BlockEntity::Rock:
		m_Model->Color(glm::vec3(1.0, 1.0, 0.0));
		break;

	case BlockEntity::Paper:
		m_Model->Color(glm::vec3(0.0, 1.0, 1.0));
		break;

	case BlockEntity::Scissors:
		m_Model->Color(glm::vec3(0.0, 1.0, 0.0));
		break;

	case BlockEntity::Red:
		m_Model->Color(glm::vec3(1.0, 0.0, 0.0));
		break;

	case BlockEntity::Blue:
		m_Model->Color(glm::vec3(0.0, 0.0, 1.0));
		break;

	case BlockEntity::Bomb:
		m_Model->Color(glm::vec3(0.15, 0.15, 0.15));
		break;
	}*/
}

void BlockEntity::Destroyed(bool p_Destroyed)
{
	// TODO: Animation
	m_Destroyed = p_Destroyed;
}

void BlockEntity::MoveToTop(int p_Y)
{
	float s_CenterX = (m_GridColumns - 1.0) / 2.0;
	float s_CenterY = (m_GridRows - 1.0) / 2.0;

	m_TargetX = (m_X - s_CenterX) * 0.30;
	m_TargetY = 0.0;
	m_TargetZ = (m_Y - s_CenterY) * 0.30;

	float s_TargetZ = (p_Y - s_CenterY) * 0.30;

	m_Model->Position(glm::vec3(m_TargetX, 0.f, s_TargetZ));

	m_Animating = true;
	m_Active = false;

	return;
}