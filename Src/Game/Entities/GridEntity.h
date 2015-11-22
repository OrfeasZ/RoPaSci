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

		private:
			BlockEntity** m_Blocks;
			int m_Columns;
			int m_Rows;
		};
	}
}