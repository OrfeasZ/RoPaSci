#pragma once

#include <Misc/StdAfx.h>

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

public:
	inline FT_Library GetFreeType() const { return m_FreeType; }
	inline GLFWwindow* GetWindow() const { return m_Window; }

protected:
	bool InitManagers();

protected:
	void OnRender();
	void OnResize(int p_Width, int p_Height);
	
protected:
	FT_Library m_FreeType;
	GLFWwindow* m_Window;
	WindowState m_CurrentState;
	int m_WindowWidth;
	int m_WindowHeight;
	int m_TickRate;
	int m_MaxFPS;

protected:
	uint32_t m_InputPostUpdateTask;
};