#include "Application.h"

#include <Util/Utils.h>
#include <VFS/FileSystem.h>

#include <Managers/TaskManager.h>
#include <Managers/ShaderManager.h>
#include <Managers/SimulationManager.h>
#include <Managers/InputManager.h>
#include <Managers/SceneManager.h>
#include <Managers/ModelManager.h>
#include <Managers/CameraManager.h>
#include <Managers/TextureManager.h>
#include <Managers/AudioManager.h>

#include <Game/Main.h>

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
	m_TickRate(256),
	m_MaxFPS(300),
	m_SimulationUpdateTask(~0)
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
	Managers::ModelManager::DestroyInstance();
	Managers::SceneManager::DestroyInstance();
	Managers::CameraManager::DestroyInstance();
	Managers::TextureManager::DestroyInstance();
	Managers::AudioManager::DestroyInstance();
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
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	
	m_Window = glfwCreateWindow(m_WindowWidth, m_WindowHeight, p_WindowTitle.c_str(), NULL, NULL);

	if (m_Window == nullptr)
	{
		Logger(Util::LogLevel::Error, "Failed to create application window. Please verify that your graphics hardware supports OpenGL and try again.");
		Shutdown();
		return;
	}

	glfwSetWindowSizeCallback(m_Window, OnResizeStatic);
	glfwMakeContextCurrent(m_Window);

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

	glViewport(0, 0, m_WindowWidth, m_WindowHeight);

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

	// Initialize the game.
	Logger(Util::LogLevel::Info, "Initializing main game logic.");
	
	if (!Game::Main::GetInstance()->Init())
	{
		Shutdown();
		return;
	}
	
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

	if (!Managers::TextureManager::GetInstance()->Init())
		return false;

	if (!Managers::ModelManager::GetInstance()->Init())
		return false;

	if (!Managers::InputManager::GetInstance()->Init())
		return false;

	if (!Managers::CameraManager::GetInstance()->Init())
		return false;

	if (!Managers::SceneManager::GetInstance()->Init())
		return false;

	if (!Managers::AudioManager::GetInstance()->Init())
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

	glfwPollEvents();

	// Simulation Update Task
	Managers::TaskManager::GetInstance()->CreateTaskSet(
		Managers::SimulationManager::GetInstance()->GetUpdateTask(),
		nullptr,
		1,
		nullptr,
		0,
		"Simulation_Update",
		&m_SimulationUpdateTask);

	Managers::TaskManager::GetInstance()->WaitForSet(m_SimulationUpdateTask);
	Managers::TaskManager::GetInstance()->ReleaseHandle(m_SimulationUpdateTask);

	// Render Task.
	Managers::SceneManager::GetInstance()->Render();

	// Sleep here so we don't kill the CPU.
	std::this_thread::sleep_for(std::chrono::microseconds(1));
}

void Application::OnResizeStatic(GLFWwindow* p_Window, int p_Width, int p_Height)
{
	GetInstance()->OnResize(p_Window, p_Width, p_Height);
}

void Application::OnResize(GLFWwindow* p_Window, int p_Width, int p_Height)
{
	m_WindowWidth = p_Width;
	m_WindowHeight = p_Height;

	glViewport(0, 0, p_Width, p_Height);

	Managers::SceneManager::GetInstance()->OnResize(p_Width, p_Height);
}
