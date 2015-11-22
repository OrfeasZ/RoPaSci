#pragma once

#include <Misc/StdAfx.h>

namespace Rendering
{
	class IRenderer
	{
	public:
		virtual ~IRenderer() {};

	public:
		virtual bool Init() = 0;
		virtual void Update(double p_Delta) = 0;
		virtual void Render(double p_Delta) = 0;
		virtual void OnResize(int p_Width, int p_Height) {}
	};
}