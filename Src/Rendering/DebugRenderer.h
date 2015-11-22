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
		virtual bool Init() override;
		virtual void Update(double p_Delta) override;
		virtual void Render(double p_Delta) override;
		
	public:
		void DrawTextString(const std::string& p_Text, float x, float y, float sx, float sy);

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


		
		GLuint m_Basic2DProgram;
		GLint m_Basic2DAttributeCoord;
		GLint m_Basic2DUniformTex;
		GLint m_Basic2DUniformColor;
	};
}