#pragma once

#include <Misc/StdAfx.h>
#include "IEntity.h"

#include <Rendering/Objects/Model.h>

namespace Game
{
	namespace Entities
	{
		class BlockEntity : public IEntity
		{
		public:
			enum BlockType
			{
				Rock,
				Paper,
				Scissors,
				Red,
				Blue,
				Bomb,
				Count
			};

		public:
			BlockEntity(BlockType p_Type, int p_X, int p_Y, int p_Columns, int p_Rows);
			virtual ~BlockEntity() override;

		public:
			virtual void Init() override;
			virtual void Update(double p_Delta) override;

		public:
			void Type(BlockType p_Type);
			void Position(int p_X, int p_Y, bool p_Simulated = false, float p_Delay = 0.f);
			void Active(bool p_Active);
			void Hover(bool p_Hover);
			void Destroyed(bool p_Destroyed);
			void MoveToTop(int p_Y, float p_Delay = 0.f);

		public:
			inline BlockType Type() const { return m_Type; }
			inline int X() const { return m_X; }
			inline int Y() const { return m_Y; }
			inline bool Animating() const { return m_AnimatingMovement || m_ScaleStep > 0.f || m_PendingPosition; }
			inline bool Destroyed() const { return m_Destroyed; }

		private:
			BlockType m_Type;
			int m_X;
			int m_Y;
			int m_GridColumns;
			int m_GridRows;
			Rendering::Objects::Model* m_Model;
			bool m_Active;
			bool m_Hover;

		private:
			float m_MovementDelay;
			bool m_Destroyed;
			bool m_AnimatingMovement;
			float m_TargetX;
			float m_TargetY;
			float m_TargetZ;
			bool m_ResetHeight;
			
			int m_PendingX;
			int m_PendingY;
			bool m_PendingPosition;

			float m_ScaleStep;
			float m_ScaleTimer;
		};
	}
}