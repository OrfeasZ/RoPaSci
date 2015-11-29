#include "LifeEntity.h"

using namespace Game::Entities;

LifeEntity::LifeEntity(uint32_t p_Lives) :
	m_Lives(p_Lives)
{
}

LifeEntity::~LifeEntity()
{
}

void LifeEntity::Init()
{
}

void LifeEntity::Update(double p_Delta)
{

}

void LifeEntity::OnLifeLost()
{
	--m_Lives;
}