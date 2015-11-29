#include "GridEntity.h"

#include "BlockEntity.h"
#include "ScoringEntity.h"
#include "LifeEntity.h"

#include <Managers/InputManager.h>
#include <Managers/AudioManager.h>

#include <Rendering/MainRenderer.h>

#include <time.h>

using namespace Game::Entities;

GridEntity::GridEntity(uint32_t p_Columns, uint32_t p_Rows, uint32_t p_Lives) :
	m_Columns(p_Columns),
	m_Rows(p_Rows),
	m_Lives(p_Lives),
	m_ActiveBlock(nullptr),
	m_HoverBlock(nullptr),
	m_PendingDestruction(false),
	m_ScoringEntity(nullptr)
{
}

GridEntity::~GridEntity()
{
	for (uint32_t i = 0; i < m_Columns; ++i)
		for (uint32_t j = 0; j < m_Rows; ++j)
			delete m_Blocks[i + (j * m_Columns)];

	free(m_Blocks);

	if (m_ScoringEntity)
		delete m_ScoringEntity;

	if (m_LifeEntity)
		delete m_LifeEntity;
}

void GridEntity::Init()
{
	m_BatchUpdateTask = std::bind(&GridEntity::BatchUpdateTask, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);

	m_ScoringEntity = new ScoringEntity();
	m_LifeEntity = new LifeEntity(m_Lives);

	m_Blocks = (BlockEntity**) malloc(m_Columns * m_Rows * sizeof(BlockEntity*));

	// Set our random seed
	srand((uint32_t) time(NULL) + (uint32_t) m_Blocks);

	for (uint32_t j = 0; j < m_Rows; ++j)
	{
		for (uint32_t i = 0; i < m_Columns; ++i)
		{
			m_Blocks[i + (j * m_Columns)] = GenerateRandomBlock(i, j);
			m_Blocks[i + (j * m_Columns)]->Init();
			m_Blocks[i + (j * m_Columns)]->MoveToTop(j - m_Columns);
		}
	}

	// Run a destruction simulation to make sure we don't already have formed groups.
	while (DestructionStep(true))
		continue;
}

void GridEntity::Update(double p_Delta)
{
	bool s_Animating = false;

	float s_Delta = (float) p_Delta;

	uint32_t s_BatchTask;
	Managers::TaskManager::GetInstance()->CreateTaskSet(
		m_BatchUpdateTask,
		*(void**) &s_Delta,
		4,
		nullptr,
		0,
		"GridEntity_BatchUpdate",
		&s_BatchTask);

	Managers::TaskManager::GetInstance()->WaitForSet(s_BatchTask);
	Managers::TaskManager::GetInstance()->ReleaseHandle(s_BatchTask);

	for (uint32_t i = 0; i < m_Rows * m_Columns; ++i)
	{
		if (m_Blocks[i] == nullptr)
			continue;

		if (m_Blocks[i]->Animating())
			s_Animating = true;
	}

	if (s_Animating)
		return;

	if (m_PendingDestruction)
	{
		m_PendingDestruction = false;
		DestructionStep(false, true);
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
		(int) Managers::InputManager::GetInstance()->GetCursorX(),
		(int) Managers::InputManager::GetInstance()->GetCursorY()
	);

	int s_CurrentX = (int) glm::floor((s_CursorPos.x - s_FarLeft) / 0.3);
	int s_CurrentY = (int) glm::floor((s_CursorPos.z - s_FarTop) / 0.3);

	if (Managers::InputManager::GetInstance()->IsMouseButtonPressed(GLFW_MOUSE_BUTTON_1))
		SetActiveBlock(s_CurrentX, s_CurrentY);

	SetHoverBlock(s_CurrentX, s_CurrentY);
}

bool GridEntity::Processing()
{
	if (m_PendingDestruction)
		return true;

	for (uint32_t i = 0; i < m_Rows * m_Columns; ++i)
	{
		if (m_Blocks[i] == nullptr)
			continue;

		if (m_Blocks[i]->Animating())
			return true;
	}

	return false;
}

void GridEntity::SetActiveBlock(uint32_t p_X, uint32_t p_Y)
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

			// Play "Swap" sound.
			Managers::AudioManager::GetInstance()->PlayAudio("swap");

			// Run destruction.
			DestructionStep();

			m_LifeEntity->OnLifeLost();

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

void GridEntity::SetHoverBlock(uint32_t p_X, uint32_t p_Y)
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

bool GridEntity::DestructionStep(bool p_Simulated /* = false */, bool p_Filler /* = false */)
{
	Logger(Util::LogLevel::Debug, "Running Grid Destruction.");

	// Tuple: x, y, size, horizontal
	std::vector<std::tuple<int, int, int, bool>> s_Groups;

	// TODO: Split this to tasks.
	// Search for horizontal groups.
	for (uint32_t j = 0; j < m_Rows; ++j)
	{
		// We always have 1 match since we start with a single block.
		int s_MatchedBlocks = 1;

		for (uint32_t i = 0; i < m_Columns; ++i)
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
	for (uint32_t i = 0; i < m_Columns; ++i)
	{
		// Start with a single tile, cluster of 1
		int s_MatchedBlocks = 1;

		for (uint32_t j = 0; j < m_Rows; ++j)
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
		DestroyGroup(s_Groups[i], p_Simulated, p_Filler);

	if (p_Simulated)
		return true;

	// Play "Explosion" sound.
	Managers::AudioManager::GetInstance()->PlayAudio("explosion");

	// Repopulate destroyed blocks.
	RepopulateBlocks();

	// Run one more destruction once we're done with this.
	m_PendingDestruction = true;

	return true;
}

void GridEntity::DestroyGroup(std::tuple<int, int, int, bool> p_Group, bool p_Simulated /* = false */, bool p_Filler /* = false */)
{
	int s_ColumnOffset = 0;
	int s_RowOffset = 0;

	auto s_BlockType = GetBlock(std::get<0>(p_Group), std::get<1>(p_Group))->Type();

	// Destroy the main blocks.
	for (int j = 0; j < std::get<2>(p_Group); ++j)
	{
		int x = std::get<0>(p_Group) + s_ColumnOffset;
		int y = std::get<1>(p_Group) + s_RowOffset;

		if (m_Blocks[x + (y * m_Columns)] != nullptr)
		{
			if (p_Simulated)
			{
				// If we're running in simulation mode just replace the color.
				m_Blocks[x + (y * m_Columns)]->Type(GetRandomBlockType());
			}
			else
			{
				// Otherwise, handle a proper block destruction along with destruction of neighboring blocks.
				m_Blocks[x + (y * m_Columns)]->Destroyed(true);

				if (p_Filler)
					m_ScoringEntity->OnFillDestroyed();
			}

		}

		if (std::get<3>(p_Group))
			++s_ColumnOffset;
		else
			++s_RowOffset;
	}

	if (p_Simulated)
		return;

	if (!p_Filler)
		m_ScoringEntity->OnGroupDestroyed();

	// Red and blue blocks don't destroy neighboring blocks.
	if (s_BlockType == BlockEntity::Blue ||
		s_BlockType == BlockEntity::Red)
		return;

	// Destroy all neighboring blocks.
	int s_StartX = std::get<0>(p_Group) - 3;
	uint32_t s_EndX = std::get<3>(p_Group) ? (std::get<0>(p_Group) +std::get<2>(p_Group) +4) : std::get<0>(p_Group) +4;

	int s_StartY = std::get<1>(p_Group) - 3;
	uint32_t s_EndY = !std::get<3>(p_Group) ? (std::get<1>(p_Group) +std::get<2>(p_Group) +4) : std::get<1>(p_Group) +4;
	
	if (s_StartX < 0)
		s_StartX = 0;

	if (s_EndX > m_Columns)
		s_EndX = m_Columns;

	if (s_StartY < 0)
		s_StartY = 0;

	if (s_EndY > m_Rows)
		s_EndY = m_Rows;

	uint32_t s_LevelOneMissed = 0;
	uint32_t s_LevelOneDestroyed = 0;
	uint32_t s_LevelTwoThreeDestroyed = 0;
	uint32_t s_BombsDestroyed = 0;

	for (uint32_t y = s_StartY; y < s_EndY; ++y)
	{
		for (uint32_t x = s_StartX; x < s_EndX; ++x)
		{
			if (m_Blocks[x + (y * m_Columns)] == nullptr)
				continue;

			if (((int) x <= std::get<0>(p_Group) - 2 || (int) x >= (std::get<3>(p_Group) ? (std::get<0>(p_Group) + std::get<2>(p_Group) + 2) : std::get<0>(p_Group) + 2)) &&
				((int) y <= std::get<1>(p_Group) - 2 || (int) y >= (!std::get<3>(p_Group) ? (std::get<1>(p_Group) + std::get<2>(p_Group) + 2) : std::get<1>(p_Group) + 2)))
			{
				// Outer two.
				if ((s_BlockType == BlockEntity::Rock && m_Blocks[x + (y * m_Columns)]->Type() == BlockEntity::Scissors) || 
					(s_BlockType == BlockEntity::Paper && m_Blocks[x + (y * m_Columns)]->Type() == BlockEntity::Rock) ||
					(s_BlockType == BlockEntity::Scissors && m_Blocks[x + (y * m_Columns)]->Type() == BlockEntity::Paper))
				{
					m_Blocks[x + (y * m_Columns)]->Destroyed(true);
					++s_LevelTwoThreeDestroyed;
				}
			}
			else
			{
				// Inner one.
				// We can't eat our opposites or ourselves.
				if ((s_BlockType == BlockEntity::Rock && m_Blocks[x + (y * m_Columns)]->Type() == BlockEntity::Paper) ||
					(s_BlockType == BlockEntity::Paper && m_Blocks[x + (y * m_Columns)]->Type() == BlockEntity::Scissors) ||
					(s_BlockType == BlockEntity::Scissors && m_Blocks[x + (y * m_Columns)]->Type() == BlockEntity::Rock) || 
					s_BlockType == m_Blocks[x + (y * m_Columns)]->Type())
				{
					++s_LevelOneMissed;
					continue;
				}

				// Did we hit a bomb?
				if (m_Blocks[x + (y * m_Columns)]->Type() == BlockEntity::Bomb)
					++s_BombsDestroyed;
				else
					++s_LevelOneDestroyed;

				m_Blocks[x + (y * m_Columns)]->Destroyed(true);
			}
		}
	}

	if (p_Filler)
	{
		// We ignore bombs here.
		m_ScoringEntity->OnFillDestroyed(s_LevelOneDestroyed + s_LevelTwoThreeDestroyed);
	}
	else
	{
		m_ScoringEntity->OnLevelOneDestroyed(s_LevelOneDestroyed, s_LevelOneMissed);
		m_ScoringEntity->OnLevelTwoThreeDestroyed(s_LevelTwoThreeDestroyed);
		m_ScoringEntity->OnBombsDestroyed(s_BombsDestroyed);
	}
}

void GridEntity::RepopulateBlocks()
{
	int* s_ColumnBlocks = new int[m_Columns];
	memset(s_ColumnBlocks, 0x00, sizeof(int) * m_Columns);

	// Go from bottom to top and shift blocks.
	for (int y = (int) m_Rows - 1; y >= 0; --y)
	{
		for (uint32_t x = 0; x < m_Columns; ++x)
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

				s_CurrentBlock->Type(GetRandomBlockType());
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

BlockEntity* GridEntity::GenerateRandomBlock(uint32_t p_X, uint32_t p_Y)
{
	return new BlockEntity(GetRandomBlockType(), p_X, p_Y, m_Columns, m_Rows);
}

BlockEntity::BlockType GridEntity::GetRandomBlockType()
{
	// Get a random block type (excluding bomb).
	auto s_Type = (BlockEntity::BlockType) (rand() % BlockEntity::Bomb);

	// There's a 10% chance the block will be a bomb.
	if ((rand() % 101) <= 10)
		s_Type = BlockEntity::Bomb;

	return s_Type;
}

void GridEntity::BatchUpdateTask(void* p_Argument, int32_t p_ContextID, uint32_t p_Index, uint32_t p_Size)
{
	float s_Delta = *(float*) &p_Argument;

	int s_Start = ((m_Rows * m_Columns) / p_Size) * p_Index;
	int s_End = s_Start + ((m_Rows * m_Columns) / p_Size);

	for (int i = s_Start; i < s_End; ++i)
	{
		if (!m_Blocks[i])
			continue;

		m_Blocks[i]->Update(s_Delta);
	}
}