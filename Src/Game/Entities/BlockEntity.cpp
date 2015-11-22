#include "BlockEntity.h"

#include <Managers/ModelManager.h>
#include <Managers/ShaderManager.h>

using namespace Game::Entities;

BlockEntity::BlockEntity(BlockType p_Type, int p_X, int p_Y, int p_Columns, int p_Rows) :
	m_Type(p_Type),
	m_Position(p_X, p_Y),
	m_GridSize(p_Columns, p_Rows),
	m_Model(nullptr)
{
	Logger(Util::LogLevel::Debug, "Creating BlockEntity of type '%d' at %fx%f.", m_Type, m_Position.x, m_Position.y);
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
	case BlockEntity::Red:
		m_Model->Color(glm::vec3(1.0, 0.0, 0.0));
		break;

	case BlockEntity::Blue:
		m_Model->Color(glm::vec3(0.0, 0.0, 1.0));
		break;
	}

	float s_CenterX = (m_GridSize.x - 1.0) / 2.0;
	float s_CenterY = (m_GridSize.y - 1.0) / 2.0;

	m_Model->Translate(glm::vec3((m_Position.x - s_CenterX) * 0.30, 0.0, (m_Position.y - s_CenterY) * 0.30));

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
	case BlockEntity::Red:
		m_Model->Color(glm::vec3(1.0, 0.0, 0.0));
		break;

	case BlockEntity::Blue:
		m_Model->Color(glm::vec3(0.0, 0.0, 1.0));
		break;
	}
}

void BlockEntity::Position(glm::vec2 p_Position)
{
	if (m_Position == p_Position)
		return;

	// TODO: This update should be interpolated.
	m_Position = p_Position; 
	m_Model->Position(glm::vec3(m_Position.x, 0.0, m_Position.y));
}