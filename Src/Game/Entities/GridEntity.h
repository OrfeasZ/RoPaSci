#pragma once

#include <Misc/StdAfx.h>
#include "IEntity.h"

namespace Game
{
	namespace Entities
	{
		class BlockEntity;

		class GridEntity : public IEntity
		{
		public:
			GridEntity(int p_Columns, int p_Rows);
			virtual ~GridEntity() override;

		public:
			virtual void Init() override;
			virtual void Update(double p_Delta) override;

		public:
			void SetActiveBlock(int p_X, int p_Y);
			void SetHoverBlock(int p_X, int p_Y);

		public:
			inline BlockEntity** GetBlocks() const { return m_Blocks; }
			inline BlockEntity* GetBlock(int p_X, int p_Y) const { return m_Blocks[p_X + (p_Y * m_Columns)]; }

			inline int GetColumns() const { return m_Columns; }
			inline int GetRows() const { return m_Rows; }

			inline BlockEntity* GetActiveBlock() const { return m_ActiveBlock; }
			inline BlockEntity* GetHoverBlock() const { return m_HoverBlock; }

		private:
			bool DestructionStep(bool p_Simulated = false);
			void DestroyGroup(std::tuple<int, int, int, bool> p_Group, bool p_Simulated = false);
			void RepopulateBlocks();
			BlockEntity* GenerateRandomBlock(int p_X, int p_Y);

		private:
			BlockEntity* m_ActiveBlock;
			BlockEntity* m_HoverBlock;
			BlockEntity** m_Blocks;
			int m_Columns;
			int m_Rows;
			bool m_PendingDestruction;
		};
	}
}