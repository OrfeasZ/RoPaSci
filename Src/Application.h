#pragma once

#include <Misc/StdAfx.h>

namespace Managers
{
	class ShaderManager;
	class SimulationManager;
	class TimingManager;
}

namespace Rendering
{
	class IRenderer;
}

class Application
{
public:
	enum WindowState
	{
		Windowed,
		Fullscreen,
		Borderless
	};

public:
	static Application* GetInstance();
	static void DestroyInstance();

private:
	static Application* m_Instance;

public:
	Application();
	~Application();

public:
	void Init(int p_WindowX, int p_WindowY, int p_WindowWidth, int p_WindowHeight, const std::string& p_WindowTitle);
	void Shutdown();

	void SetWindowState(WindowState p_State);
	void SetTickRate(int p_TickRate);
	void SetMaxFPS(int p_MaxFPS);
	void RegisterRenderer(Rendering::IRenderer* p_Renderer);

public:
	inline Managers::ShaderManager* GetShaderManager() const { return m_ShaderManager; }

protected:
	static void OnRenderStatic();
	static void OnKeyboardStatic(unsigned char p_Key, int p_X, int p_Y);
	static void OnKeyboardUpStatic(unsigned char p_Key, int p_X, int p_Y);
	static void OnSpecialStatic(int p_Key, int p_X, int p_Y);
	static void OnSpecialUpStatic(int p_Key, int p_X, int p_Y);
	static void OnResizeStatic(int p_Width, int p_Height);
	static void OnIdleStatic();

protected:
	bool InitManagers();
	bool InitRenderers();

protected:
	void OnRender();
	void OnKeyboard(unsigned char p_Key, int p_X, int p_Y);
	void OnKeyboardUp(unsigned char p_Key, int p_X, int p_Y);
	void OnSpecial(int p_Key, int p_X, int p_Y);
	void OnSpecialUp(int p_Key, int p_X, int p_Y);
	void OnResize(int p_Width, int p_Height);
	void OnIdle();

protected:
	Managers::ShaderManager* m_ShaderManager;
	Managers::TimingManager* m_RenderTimingManager;
	Managers::SimulationManager* m_SimulationManager;

protected:
	std::vector<Rendering::IRenderer*> m_Renderers;

protected:
	WindowState m_CurrentState;
	int m_WindowWidth;
	int m_WindowHeight;
	int m_TickRate;
	int m_MaxFPS;
};