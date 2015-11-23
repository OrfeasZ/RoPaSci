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
	m_Hover(false)
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
		m_Model->Color(glm::vec3(0.15, 0.15, 0.15));
		break;
	}

	float s_CenterX = (m_GridColumns - 1.0) / 2.0;
	float s_CenterY = (m_GridRows - 1.0) / 2.0;

	m_Model->Translate(glm::vec3((m_X - s_CenterX) * 0.30, 0.0, (m_Y - s_CenterY) * 0.30));

	Managers::ModelManager::GetInstance()->RegisterModel(m_Model);
}

void BlockEntity::Update(double p_Delta)
{

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

void BlockEntity::Position(int p_X, int p_Y)
{
	if (m_X == p_X && m_Y == p_Y)
		return;

	// TODO: This update should be interpolated.
	m_Y = p_Y;
	m_X = p_X;

	float s_CenterX = (m_GridColumns - 1.0) / 2.0;
	float s_CenterY = (m_GridRows - 1.0) / 2.0;

	m_Model->Position(glm::vec3((m_X - s_CenterX) * 0.30, 0.0, (m_Y - s_CenterY) * 0.30));
}

void BlockEntity::Active(bool p_Active)
{
	m_Active = p_Active;

	if (m_Active)
	{
		m_Model->Color(glm::vec3(1.0, 0.0, 1.0));
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
	}
}

void BlockEntity::Hover(bool p_Hover)
{
	m_Hover = p_Hover;

	if (m_Active)
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
	}
}