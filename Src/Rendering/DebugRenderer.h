#pragma once

#include "IRenderer.h"

#include <atomic>

namespace Rendering
{
	class DebugRenderer : public IRenderer
	{
	public:
		DebugRenderer();
		virtual ~DebugRenderer() override;

	public:
		virtual void Update(double p_Delta);
		virtual void Render(double p_Delta);

	private:
		// Update functions.
		void CalculateFPS(double p_Delta);

	private:
		// Render functions.
		void DrawFPS(double p_Delta);

	protected:
		int m_Frames;
		double m_BaseTime;
		double m_UpdateTime;
		double m_CurrentFramerate;
		char m_Framerate[64];
	};
}