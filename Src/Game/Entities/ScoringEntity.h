#pragma once

#include <Misc/StdAfx.h>

#include "IEntity.h"

namespace Game
{
	namespace Entities
	{
		class ScoringEntity : public IEntity
		{
		public:
			ScoringEntity();
			~ScoringEntity();

		public:
			virtual void Init() override;
			virtual void Update(double p_Delta) override;

		public:
			void OnGroupDestroyed();
			void OnLevelOneDestroyed(uint32_t p_Destroyed, uint32_t p_NotDestroyed);
			void OnLevelTwoThreeDestroyed(uint32_t p_Destroyed);
			void OnBombsDestroyed(uint32_t p_Destroyed);
			void OnFillDestroyed(uint32_t p_Destroyed = 1);

			void ResetScore();

		public:
			inline int64_t GetScore() const { return m_Score; }

		private:
			int64_t m_Score;
		};
	}
}