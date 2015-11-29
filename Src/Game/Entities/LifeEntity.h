#pragma once

#include <Misc/StdAfx.h>

#include "IEntity.h"

namespace Game
{
	namespace Entities
	{
		class LifeEntity : public IEntity
		{
		public:
			LifeEntity(uint32_t p_Lives);
			~LifeEntity();

		public:
			virtual void Init() override;
			virtual void Update(double p_Delta) override;

		public:
			void OnLifeLost();

		public:
			inline uint32_t GetLives() const { return m_Lives; }

		private:
			uint32_t m_Lives;
		};
	}
}