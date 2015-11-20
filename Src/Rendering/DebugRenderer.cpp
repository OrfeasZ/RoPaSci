#include "DebugRenderer.h"

using namespace Rendering;

DebugRenderer::DebugRenderer() :
	m_Frames(0),
	m_BaseTime(0.0),
	m_CurrentFramerate(0.0),
	m_UpdateTime(0.0)
{

}

DebugRenderer::~DebugRenderer()
{

}

void DebugRenderer::Update(double p_Delta)
{
	CalculateFPS(p_Delta);
}

void DebugRenderer::Render(double p_Delta)
{
	DrawFPS(p_Delta);
}

void DebugRenderer::CalculateFPS(double p_Delta)
{
	m_UpdateTime += p_Delta;

	if (m_UpdateTime > 0.25)
	{
		m_UpdateTime = 0.0;
		sprintf(m_Framerate, "FPS: %04.2f", m_CurrentFramerate);
	}
}

void DebugRenderer::DrawFPS(double p_Delta)
{
	++m_Frames;

	m_BaseTime += p_Delta;

	if (m_BaseTime > 0.25)
	{
		m_CurrentFramerate = (double) m_Frames / m_BaseTime;
		m_BaseTime = m_Frames = 0;
	}

	glColor3f(0.0f, 1.0f, 0.4f);

	//  Specify the raster position for pixel operations.
	glRasterPos3f(-0.95f, 0.9f, 0.f);

	//  Draw the characters one by one
	for (int i = 0; m_Framerate[i] != '\0'; ++i)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, m_Framerate[i]);
}