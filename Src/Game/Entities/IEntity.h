#pragma once

#include <Misc/StdAfx.h>

namespace Game
{
	namespace Entities
	{
		class IEntity
		{
		public:
			virtual ~IEntity() {}

		public:
			virtual void Init() = 0;
			virtual void Update(double p_Delta) = 0;
		};
	}
}