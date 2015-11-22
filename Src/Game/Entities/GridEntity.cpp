#include "GridEntity.h"

#include "BlockEntity.h"

using namespace Game::Entities;

GridEntity::GridEntity(int p_Columns, int p_Rows) :
	m_Columns(p_Columns),
	m_Rows(p_Rows)
{

}

GridEntity::~GridEntity()
{
	for (int i = 0; i < m_Columns; ++i)
		for (int j = 0; j < m_Rows; ++j)
			delete m_Blocks[i * j];

	free(m_Blocks);
}

void GridEntity::Init()
{
	m_Blocks = (BlockEntity**) malloc(m_Columns * m_Rows * sizeof(BlockEntity*));

	// TODO: Random block population.
	for (int i = 0; i < m_Columns; ++i)
	{
		for (int j = 0; j < m_Rows; ++j)
		{
			m_Blocks[i * j] = new BlockEntity(BlockEntity::Red, i, j, m_Columns, m_Rows);
			m_Blocks[i * j]->Init();
		}
	}

	m_Blocks[5]->Type(BlockEntity::Blue);
	m_Blocks[15]->Type(BlockEntity::Blue);
	m_Blocks[60]->Type(BlockEntity::Blue);
}

void GridEntity::Update(double p_Delta)
{

}