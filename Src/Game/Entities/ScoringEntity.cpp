#include "ScoringEntity.h"

using namespace Game::Entities;

ScoringEntity::ScoringEntity() :
	m_Score(0)
{
}

ScoringEntity::~ScoringEntity()
{
}

void ScoringEntity::Init()
{
	m_Score = 0;
}

void ScoringEntity::Update(double p_Delta)
{

}

void ScoringEntity::OnGroupDestroyed()
{
	m_Score += 10;
}

void ScoringEntity::OnLevelOneDestroyed(uint32_t p_Destroyed, uint32_t p_NotDestroyed)
{
	m_Score += (p_Destroyed * 2);
	m_Score -= p_NotDestroyed;
}

void ScoringEntity::OnLevelTwoThreeDestroyed(uint32_t p_Destroyed)
{
	m_Score += (p_Destroyed * 3);
}

void ScoringEntity::OnBombsDestroyed(uint32_t p_Destroyed)
{
	m_Score -= (30 * p_Destroyed);
}

void ScoringEntity::OnFillDestroyed(uint32_t p_Destroyed)
{
	m_Score += (p_Destroyed * 2);
}

void ScoringEntity::ResetScore()
{
	m_Score = 0;
}