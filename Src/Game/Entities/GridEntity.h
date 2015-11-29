#pragma once

#include <Misc/StdAfx.h>

#include "IEntity.h"
#include "BlockEntity.h"

namespace Game
{
	namespace Entities
	{
		class ScoringEntity;
		class LifeEntity;

		class GridEntity : public IEntity
		{
		public:
			GridEntity(uint32_t p_Columns, uint32_t p_Rows, uint32_t p_Lives);
			virtual ~GridEntity() override;

		public:
			virtual void Init() override;
			virtual void Update(double p_Delta) override;

		public:
			void SetActiveBlock(uint32_t p_X, uint32_t p_Y);
			void SetHoverBlock(uint32_t p_X, uint32_t p_Y);
			bool Processing();

		public:
			inline BlockEntity** GetBlocks() const { return m_Blocks; }
			inline BlockEntity* GetBlock(uint32_t p_X, uint32_t p_Y) const { return m_Blocks[p_X + (p_Y * m_Columns)]; }

			inline uint32_t GetColumns() const { return m_Columns; }
			inline uint32_t GetRows() const { return m_Rows; }

			inline BlockEntity* GetActiveBlock() const { return m_ActiveBlock; }
			inline BlockEntity* GetHoverBlock() const { return m_HoverBlock; }
			inline ScoringEntity* GetScoringEntity() const { return m_ScoringEntity; }
			inline LifeEntity* GetLifeEntity() const { return m_LifeEntity; }

		private:
			bool DestructionStep(bool p_Simulated = false, bool p_Filler = false);
			void DestroyGroup(std::tuple<int, int, int, bool> p_Group, bool p_Simulated = false, bool p_Filler = false);
			void RepopulateBlocks();
			BlockEntity* GenerateRandomBlock(uint32_t p_X, uint32_t p_Y);
			BlockEntity::BlockType GetRandomBlockType();

		private:
			BlockEntity* m_ActiveBlock;
			BlockEntity* m_HoverBlock;
			BlockEntity** m_Blocks;
			ScoringEntity* m_ScoringEntity;
			LifeEntity* m_LifeEntity;
			uint32_t m_Columns;
			uint32_t m_Rows;
			uint32_t m_Lives;
			bool m_PendingDestruction;
		};
	}
}