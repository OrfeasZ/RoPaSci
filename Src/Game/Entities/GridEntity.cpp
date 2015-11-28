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
	m_HoverBlock(nullptr),
	m_PendingDestruction(false)
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
			m_Blocks[i + (j * m_Columns)] = GenerateRandomBlock(i, j);
			m_Blocks[i + (j * m_Columns)]->Init();
		}
	}

	// Run a destruction simulation to make sure we don't already have formed groups.
	while (DestructionStep(true))
		continue;
}

void GridEntity::Update(double p_Delta)
{
	bool s_Animating = false;

	// TODO: Turn these updates into tasks.
	for (int i = 0; i < m_Rows * m_Columns; ++i)
	{
		if (m_Blocks[i] == nullptr)
			continue;

		m_Blocks[i]->Update(p_Delta);

		if (m_Blocks[i]->Animating())
			s_Animating = true;
	}

	if (s_Animating)
		return;

	if (m_PendingDestruction)
	{
		m_PendingDestruction = false;
		DestructionStep();
		return;
	}

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

			// Run destruction.
			DestructionStep();

			return;
		}

		// De-activate the currently active block.
		m_ActiveBlock->Active(false);
	}

	// Activate the newly selected block.
	m_ActiveBlock = GetBlock(p_X, p_Y);

	if (m_ActiveBlock == nullptr)
		return;

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

bool GridEntity::DestructionStep(bool p_Simulated /* = false */)
{
	Logger(Util::LogLevel::Debug, "Running Grid Destruction.");

	// Tuple: x, y, size, horizontal
	std::vector<std::tuple<int, int, int, bool>> s_Groups;

	// TODO: Split this to tasks.
	// Search for horizontal groups.
	for (int j = 0; j < m_Rows; ++j) 
	{
		// We always have 1 match since we start with a single block.
		int s_MatchedBlocks = 1;

		for (int i = 0; i < m_Columns; ++i) 
		{
			bool s_Check = false;

			// Is this the last tile?
			if (i == m_Columns - 1) 
			{
				s_Check = true;
			}
			else 
			{
				// Make sure the block types match while ignoring bombs.
				if (GetBlock(i, j) != nullptr && GetBlock(i + 1, j) != nullptr &&
					GetBlock(i, j)->Type() == GetBlock(i + 1, j)->Type() && 
					GetBlock(i, j)->Type() != BlockEntity::Bomb)
					s_MatchedBlocks += 1;
				else
					s_Check = true;
			}

			// Did we have a group?
			if (s_Check) 
			{
				// We did! Add it to our collection.
				if (s_MatchedBlocks >= 3) 
					s_Groups.push_back(std::make_tuple(i + 1 - s_MatchedBlocks, j, s_MatchedBlocks, true));

				s_MatchedBlocks = 1;
			}
		}
	}

	// Do the same for vertical groups.
	for (int i = 0; i < m_Columns; ++i) 
	{
		// Start with a single tile, cluster of 1
		int s_MatchedBlocks = 1;

		for (int j = 0; j < m_Rows; ++j) 
		{
			bool s_Check = false;

			if (j == m_Rows - 1) 
			{
				s_Check = true;
			}
			else
			{
				if (GetBlock(i, j) != nullptr && GetBlock(i, j + 1) != nullptr &&
					GetBlock(i, j)->Type() == GetBlock(i, j + 1)->Type() && 
					GetBlock(i, j)->Type() != BlockEntity::Bomb)
					s_MatchedBlocks += 1;
				else 
					s_Check = true;
			}

			if (s_Check)
			{
				if (s_MatchedBlocks >= 3) 
					s_Groups.push_back(std::make_tuple(i, j + 1 - s_MatchedBlocks, s_MatchedBlocks, false));

				s_MatchedBlocks = 1;
			}
		}
	}

	if (s_Groups.size() == 0)
		return false;

	// Destroy blocks.
	for (size_t i = 0; i < s_Groups.size(); ++i) 
		DestroyGroup(s_Groups[i], p_Simulated);

	if (p_Simulated)
		return true;

	// Repopulate destroyed blocks.
	RepopulateBlocks();

	// Run one more destruction once we're done with this.
	m_PendingDestruction = true;

	return true;
}

void GridEntity::DestroyGroup(std::tuple<int, int, int, bool> p_Group, bool p_Simulated /* = false */)
{
	int s_ColumnOffset = 0;
	int s_RowOffset = 0;

	auto s_BlockType = GetBlock(std::get<0>(p_Group), std::get<1>(p_Group))->Type();

	// Destroy the main blocks.
	for (int j = 0; j < std::get<2>(p_Group); ++j)
	{
		int x = std::get<0>(p_Group) +s_ColumnOffset;
		int y = std::get<1>(p_Group) +s_RowOffset;

		if (m_Blocks[x + (y * m_Columns)] != nullptr)
		{
			if (p_Simulated)
			{
				// If we're running in simulation mode just replace the color.
				m_Blocks[x + (y * m_Columns)]->Type((BlockEntity::BlockType) (rand() % BlockEntity::Count));
			}
			else
			{
				// Otherwise, handle a proper block destruction along with destruction of neighboring blocks.
				// TODO: Scoring.
				m_Blocks[x + (y * m_Columns)]->Destroyed(true);
			}

		}

		if (std::get<3>(p_Group))
			++s_ColumnOffset;
		else
			++s_RowOffset;
	}

	if (p_Simulated)
		return;

	// Red and blue blocks don't destroy neighboring blocks.
	if (s_BlockType == BlockEntity::Blue ||
		s_BlockType == BlockEntity::Red)
		return;

	// Destroy all neighboring blocks.
	int s_StartX = std::get<0>(p_Group) - 3;
	int s_EndX = std::get<3>(p_Group) ? (std::get<0>(p_Group) + std::get<2>(p_Group) + 4) : std::get<0>(p_Group) + 4;

	int s_StartY = std::get<1>(p_Group) - 3;
	int s_EndY = !std::get<3>(p_Group) ? (std::get<1>(p_Group) + std::get<2>(p_Group) + 4) : std::get<1>(p_Group) + 4;
	
	if (s_StartX < 0)
		s_StartX = 0;

	if (s_EndX > m_Columns)
		s_EndX = m_Columns;

	if (s_StartY < 0)
		s_StartY = 0;

	if (s_EndY > m_Rows)
		s_EndY = m_Rows;

	for (int y = s_StartY; y < s_EndY; ++y)
	{
		for (int x = s_StartX; x < s_EndX; ++x)
		{
			if (m_Blocks[x + (y * m_Columns)] == nullptr)
				continue;

			if (s_BlockType == m_Blocks[x + (y * m_Columns)]->Type())
				continue;

			if (s_BlockType == BlockEntity::Rock && m_Blocks[x + (y * m_Columns)]->Type() == BlockEntity::Paper)
				continue;

			if (s_BlockType == BlockEntity::Paper && m_Blocks[x + (y * m_Columns)]->Type() == BlockEntity::Scissors)
				continue;

			if (s_BlockType == BlockEntity::Scissors && m_Blocks[x + (y * m_Columns)]->Type() == BlockEntity::Rock)
				continue;

			// TODO
			// TODO: Scoring.
			m_Blocks[x + (y * m_Columns)]->Destroyed(true);
		}
	}
}

void GridEntity::RepopulateBlocks()
{
	int* s_ColumnBlocks = new int[m_Columns];
	memset(s_ColumnBlocks, 0x00, sizeof(int) * m_Columns);

	// Go from bottom to top and shift blocks.
	for (int y = m_Rows - 1; y >= 0; --y)
	{
		for (int x = 0; x < m_Columns; ++x)
		{
			auto s_CurrentBlock = m_Blocks[x + (y * m_Columns)];

			// Check if block is destroyed.
			if (!s_CurrentBlock->Destroyed())
				continue;

			// Find a block in the same column to replace this block.
			BlockEntity* s_ReplacementBlock = nullptr;

			for (int i = y - 1; i >= 0; --i)
			{
				if (m_Blocks[x + (i * m_Columns)]->Destroyed())
					continue;

				s_ReplacementBlock = m_Blocks[x + (i * m_Columns)];
				break;
			}

			// No replacement block found; re-create the current one.
			if (s_ReplacementBlock == nullptr)
			{
				--s_ColumnBlocks[x];

				// TODO: Weight this differently.
				s_CurrentBlock->Type((BlockEntity::BlockType) (rand() % BlockEntity::Count));
				s_CurrentBlock->Destroyed(false);
				s_CurrentBlock->MoveToTop(s_ColumnBlocks[x], 0.4f);
				
				continue;
			}

			// Replacement block found; replace it.
			m_Blocks[s_ReplacementBlock->X() + (s_ReplacementBlock->Y() * m_Columns)] = s_CurrentBlock;
			s_CurrentBlock->Position(s_ReplacementBlock->X(), s_ReplacementBlock->Y(), true);

			m_Blocks[x + (y * m_Columns)] = s_ReplacementBlock;
			s_ReplacementBlock->Position(x, y, false, 0.4f);
		}
	}

	delete [] s_ColumnBlocks;
}

BlockEntity* GridEntity::GenerateRandomBlock(int p_X, int p_Y)
{
	// TODO: Different randomization factors / weights per different block types.
	return new BlockEntity((BlockEntity::BlockType) (rand() % BlockEntity::Count), p_X, p_Y, m_Columns, m_Rows);
}