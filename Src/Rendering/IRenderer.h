#pragma once

#include <Misc/StdAfx.h>

namespace Rendering
{
	class IRenderer
	{
	public:
		virtual ~IRenderer() {};

	public:
		virtual void Update(double p_Delta) = 0;
		virtual void Render(double p_Delta) = 0;
	};
}