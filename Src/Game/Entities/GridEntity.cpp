#include "GridEntity.h"

#include "BlockEntity.h"

#include <Managers/InputManager.h>
#include <Rendering/MainRenderer.h>

#include <time.h>

using namespace Game::Entities;

GridEntity::GridEntity(int p_Columns, int p_Rows) :
	m_Columns(p_Columns),
	m_Rows(p_Rows),
	m_ActiveBlock(nullptr),
	m_HoverBlock(nullptr)
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

	// Set our random seed
	srand(time(NULL) + (uint32_t) m_Blocks);

	for (int j = 0; j < m_Rows; ++j)
	{
		for (int i = 0; i < m_Columns; ++i)
		{
			// TODO: Different weights per block type.
			m_Blocks[i + (j * m_Columns)] = new BlockEntity((BlockEntity::BlockType) (rand() % BlockEntity::Count), i, j, m_Columns, m_Rows);
			m_Blocks[i + (j * m_Columns)]->Init();
		}
	}

	// TODO: Destroy neighboring blocks.
}

void GridEntity::Update(double p_Delta)
{
	bool s_Animating = false;

	// TODO: Turn these updates into tasks.
	for (int i = 0; i < m_Rows * m_Columns; ++i)
	{
		m_Blocks[i]->Update(p_Delta);

		if (m_Blocks[i]->Animating())
			s_Animating = true;
	}

	if (s_Animating)
		return;

	double s_CenterX = (m_Columns - 1.0) / 2.0;
	double s_CenterY = (m_Rows - 1.0) / 2.0;

	double s_FarLeft = -s_CenterX * 0.30;
	s_FarLeft -= 0.15;

	double s_FarRight = ((m_Columns - 1) - s_CenterX) * 0.30;
	s_FarRight += 0.15;

	double s_FarTop = -s_CenterY * 0.30;
	s_FarTop -= 0.15;

	double s_FarBottom = ((m_Rows - 1) - s_CenterY) * 0.30;
	s_FarBottom += 0.15;

	auto s_CursorPos = Rendering::MainRenderer::GetInstance()->ScreenToWorld(
		Managers::InputManager::GetInstance()->GetCursorX(),
		Managers::InputManager::GetInstance()->GetCursorY()
	);

	int s_CurrentX = glm::floor((s_CursorPos.x - s_FarLeft) / 0.3);
	int s_CurrentY = glm::floor((s_CursorPos.z - s_FarTop) / 0.3);

	if (Managers::InputManager::GetInstance()->IsMouseButtonPressed(GLFW_MOUSE_BUTTON_1))
		SetActiveBlock(s_CurrentX, s_CurrentY);

	SetHoverBlock(s_CurrentX, s_CurrentY);
}

void GridEntity::SetActiveBlock(int p_X, int p_Y)
{
	if (p_X < 0 || p_X >= m_Columns || p_Y < 0 || p_Y >= m_Rows)
	{
		// We have an active block; de-activate it.
		if (m_ActiveBlock != nullptr)
		{
			m_ActiveBlock->Active(false);
			m_ActiveBlock = nullptr;
		}

		return;
	}

	if (m_ActiveBlock != nullptr && m_ActiveBlock->X() == p_X && m_ActiveBlock->Y() == p_Y)
		return;

	if (m_ActiveBlock != nullptr)
	{
		// We already have an active block and we just clicked on a neighboring block; switch them.
		if (((m_ActiveBlock->X() + 1 == p_X || m_ActiveBlock->X() - 1 == p_X) && m_ActiveBlock->Y() == p_Y) || 
			((m_ActiveBlock->Y() + 1 == p_Y || m_ActiveBlock->Y() - 1 == p_Y) && m_ActiveBlock->X() == p_X))
		{
			// Set the position of the switched block.
			auto s_SwitchedBlock = GetBlock(p_X, p_Y);
			s_SwitchedBlock->Position(m_ActiveBlock->X(), m_ActiveBlock->Y());

			// Switch blocks in the grid.
			m_Blocks[p_X + (p_Y * m_Columns)] = m_ActiveBlock;
			m_Blocks[m_ActiveBlock->X() + (m_ActiveBlock->Y() * m_Columns)] = s_SwitchedBlock;

			// Set position of the active block and de-activate it.
			m_ActiveBlock->Position(p_X, p_Y);
			m_ActiveBlock = nullptr;

			return;
		}

		// De-activate the currently active block.
		m_ActiveBlock->Active(false);
	}

	// Activate the newly selected block.
	m_ActiveBlock = GetBlock(p_X, p_Y);
	m_ActiveBlock->Active(true);
}

void GridEntity::SetHoverBlock(int p_X, int p_Y)
{
	if (p_X < 0 || p_X >= m_Columns || p_Y < 0 || p_Y >= m_Rows)
	{
		if (m_HoverBlock != nullptr)
			m_HoverBlock->Hover(false);

		m_HoverBlock = nullptr;
		return;
	}

	if (m_HoverBlock != nullptr && m_HoverBlock->X() == p_X && m_HoverBlock->Y() == p_Y)
		return;

	if (m_ActiveBlock != nullptr && m_ActiveBlock->X() == p_X && m_ActiveBlock->Y() == p_Y)
		return;

	if (m_HoverBlock != nullptr)
		m_HoverBlock->Hover(false);

	m_HoverBlock = GetBlock(p_X, p_Y);
	m_HoverBlock->Hover(true);
}