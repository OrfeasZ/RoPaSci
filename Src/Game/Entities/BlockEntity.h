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
				Bomb
			};

		public:
			BlockEntity(BlockType p_Type, int p_X, int p_Y, int p_Columns, int p_Rows);
			virtual ~BlockEntity() override;

		public:
			virtual void Init() override;
			virtual void Update(double p_Delta) override;

		public:
			void Type(BlockType p_Type);
			void Position(glm::vec2 p_Position);

		public:
			inline BlockType Type() const { return m_Type; }
			inline glm::vec2 Position() const { return m_Position; }

		private:
			BlockType m_Type;
			glm::vec2 m_Position;
			glm::vec2 m_GridSize;
			Rendering::Objects::Model* m_Model;
		};
	}
}