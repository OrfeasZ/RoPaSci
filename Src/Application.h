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
	void Init(int p_WindowWidth, int p_WindowHeight, const std::string& p_WindowTitle);
	void Shutdown();

	void SetWindowState(WindowState p_State);
	void SetTickRate(int p_TickRate);
	void SetMaxFPS(int p_MaxFPS);
	bool RegisterRenderer(Rendering::IRenderer* p_Renderer);

public:
	inline Managers::ShaderManager* GetShaderManager() const { return m_ShaderManager; }
	inline Managers::TimingManager* GetRenderTimingManager() const { return m_RenderTimingManager; }
	inline Managers::SimulationManager* GetSimulationManager() const { return m_SimulationManager; }
	inline FT_Library GetFreeType() const { return m_FreeType; }
	inline GLFWwindow* GetWindow() const { return m_Window; }

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

protected:
	Managers::ShaderManager* m_ShaderManager;
	Managers::TimingManager* m_RenderTimingManager;
	Managers::SimulationManager* m_SimulationManager;

protected:
	std::vector<Rendering::IRenderer*> m_Renderers;

protected:
	FT_Library m_FreeType;
	GLFWwindow* m_Window;
	WindowState m_CurrentState;
	int m_WindowWidth;
	int m_WindowHeight;
	int m_TickRate;
	int m_MaxFPS;
};