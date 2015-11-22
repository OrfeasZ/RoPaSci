#include "DebugRenderer.h"

#include <Application.h>
#include <VFS/FileSystem.h>
#include <Managers/ShaderManager.h>

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

bool DebugRenderer::Init()
{
	Logger(Util::LogLevel::Info, "Initializing the Debug Renderer.");

	// Init our debug font.
	std::string s_FontData;
	if (!VFS::FileSystem::GetInstance()->ReadFile("/data/fonts/DebugFont.ttf", s_FontData))
	{
		Logger(Util::LogLevel::Warn, "Could not read debug font file.");
		return false;
	}

	return true;
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
}

void DebugRenderer::DrawTextString(const std::string& p_Text, float x, float y, float sx, float sy)
{
	
}