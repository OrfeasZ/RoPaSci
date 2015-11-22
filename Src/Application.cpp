#include "Application.h"

#include <Util/Utils.h>
#include <VFS/FileSystem.h>

#include <Managers/TaskManager.h>
#include <Managers/ShaderManager.h>
#include <Managers/SimulationManager.h>
#include <Managers/InputManager.h>
#include <Managers/SceneManager.h>

Application* Application::m_Instance = nullptr;

Application* Application::GetInstance()
{
	if (!m_Instance)
		m_Instance = new Application();

	return m_Instance;
}

void Application::DestroyInstance()
{
	if (!m_Instance)
		return;

	delete m_Instance;
	m_Instance = nullptr;
}

Application::Application() :
	m_CurrentState(Application::Windowed),
	m_TickRate(128),
	m_MaxFPS(300),
	m_InputPostUpdateTask(~0),
	m_SceneRenderTask(~0)
{

}

Application::~Application()
{
	if (m_Window)
		glfwDestroyWindow(m_Window);

	glfwTerminate();

	m_Window = nullptr;

	Managers::TaskManager::DestroyInstance();
	Managers::InputManager::DestroyInstance();
	Managers::SimulationManager::DestroyInstance();
	Managers::ShaderManager::DestroyInstance();
	Managers::SceneManager::DestroyInstance();
}

void Application::Init(int p_WindowWidth, int p_WindowHeight, const std::string& p_WindowTitle)
{
	m_WindowWidth = p_WindowWidth;
	m_WindowHeight = p_WindowHeight;

	// Initialize the Virtual FileSystem.
	VFS::FileSystem::GetInstance()->Init(Util::Utils::GetCurrentDir());

	// Initialize logging.
#ifdef _DEBUG
	Util::Utils::AllocateConsole(p_WindowTitle);
	Util::Log::GetInstance()->Init(Util::LogLevel::All, "RoPaSci", false);
#else
	Util::Log::GetInstance()->Init(Util::LogLevel::Warn, "RoPaSci", false);
#endif

	Logger(Util::LogLevel::Info, "Starting application...");

	// Initialize FreeType.
	if (FT_Init_FreeType(&m_FreeType))
	{
		Logger(Util::LogLevel::Error, "Failed to initialize FreeType.");
		return;
	}

	// Initialize GLFW and create our window.
	if (!glfwInit())
	{
		Logger(Util::LogLevel::Error, "Failed to initialize GLFW.");
		return;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	
	m_Window = glfwCreateWindow(m_WindowWidth, m_WindowHeight, p_WindowTitle.c_str(), NULL, NULL);

	if (m_Window == nullptr)
	{
		Logger(Util::LogLevel::Error, "Failed to create application window. Please verify that your graphics hardware supports OpenGL and try again.");
		Shutdown();
		return;
	}

	glfwMakeContextCurrent(m_Window);
	glfwSwapInterval(1);

	// Initialize glew.
	glewExperimental = true;

	if (glewInit() != GLEW_OK)
	{
		Logger(Util::LogLevel::Error, "Failed to initialize GLEW.");
		Shutdown();
		return;
	}

	if (!glewIsSupported("GL_VERSION_4_0"))
	{
		Logger(Util::LogLevel::Error, "This application requires at least OpenGL version 4.0.");
		Shutdown();
		return;
	}

	Logger(Util::LogLevel::Info, "Running OpenGL %s on %s %s.", glGetString(GL_VERSION), glGetString(GL_VENDOR), glGetString(GL_RENDERER));
	Logger(Util::LogLevel::Info, "Supported GLSL version: %s.", glGetString(GL_SHADING_LANGUAGE_VERSION));
	Logger(Util::LogLevel::Info, "GLEW %s context successfully initialized!", glewGetString(GLEW_VERSION));

	// Initialize our managers.
	if (!InitManagers())
	{
		Shutdown();
		return;
	}

	// Set max framerate and tick rate.
	Managers::SceneManager::GetInstance()->SetMaxFPS(m_MaxFPS);
	Managers::SimulationManager::GetInstance()->SetTickRate(m_TickRate);
	
	while (!glfwWindowShouldClose(m_Window))
		OnRender();

	// After we're done we should shut down.
	Shutdown();
}

void Application::Shutdown()
{
	DestroyInstance();
}

bool Application::InitManagers()
{
	if (!Managers::TaskManager::GetInstance()->Init())
		return false;

	if (!Managers::ShaderManager::GetInstance()->Init())
		return false;

	if (!Managers::InputManager::GetInstance()->Init())
		return false;

	if (!Managers::SceneManager::GetInstance()->Init())
		return false;

	if (!Managers::SimulationManager::GetInstance()->Init())
		return false;

	return true;
}

void Application::SetTickRate(int p_TickRate)
{
	m_TickRate = p_TickRate;
	Managers::SimulationManager::GetInstance()->SetTickRate(m_TickRate);
}

void Application::SetMaxFPS(int p_MaxFPS)
{
	m_MaxFPS = p_MaxFPS;
	Managers::SceneManager::GetInstance()->SetMaxFPS(m_MaxFPS);
}

void Application::OnRender()
{
	if (!m_Window)
		return;

	// Create our basic task list.
	// TODO: Simulation task.

	if (m_SceneRenderTask != ~0)
	{
		Managers::TaskManager::GetInstance()->WaitForSet(m_SceneRenderTask);
		Managers::TaskManager::GetInstance()->ReleaseHandle(m_SceneRenderTask);
	}

	// Input PostUpdate Task
	Managers::TaskManager::GetInstance()->CreateTaskSet(
		Managers::InputManager::GetInstance()->GetPostUpdateTask(),
		nullptr,
		1,
		nullptr,
		0,
		"Input_PostUpdate",
		&m_InputPostUpdateTask);

	// Render Task.
	Managers::TaskManager::GetInstance()->CreateTaskSet(
		Managers::SceneManager::GetInstance()->GetRenderTask(),
		nullptr,
		1,
		nullptr,
		0,
		"Input_PostUpdate",
		&m_SceneRenderTask);

	Managers::TaskManager::GetInstance()->WaitForSet(m_InputPostUpdateTask);
	Managers::TaskManager::GetInstance()->ReleaseHandle(m_InputPostUpdateTask);
}

void Application::OnResize(int p_Width, int p_Height)
{

}
