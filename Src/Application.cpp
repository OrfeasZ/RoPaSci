#include "Application.h"

#include <Util/Utils.h>
#include <VFS/FileSystem.h>

#include <Managers/TaskManager.h>
#include <Managers/ShaderManager.h>
#include <Managers/SimulationManager.h>
#include <Managers/InputManager.h>
#include <Managers/SceneManager.h>

#include <Rendering/DebugRenderer.h>

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
}

Application::Application() :
	m_CurrentState(Application::Windowed),
	m_TickRate(120),
	m_MaxFPS(300)
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

	for (auto s_Renderer : m_Renderers)
		delete s_Renderer;

	m_Renderers.clear();
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

	// Enable the features we want to use.
	//glEnable(GL_DEPTH_TEST);
	//glDepthFunc(GL_LESS);

	glEnable(GL_CULL_FACE); 
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Initialize our managers.
	if (!InitManagers())
	{
		Shutdown();
		return;
	}

	Logger(Util::LogLevel::Info, "Initializing renderer...");

	// Initialize our renderers.
	if (!InitRenderers())
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

	if (!Managers::SimulationManager::GetInstance()->Init())
		return false;

	if (!Managers::InputManager::GetInstance()->Init(m_Window))
		return false;

	if (!Managers::SceneManager::GetInstance()->Init())
		return false;

	return true;
}

bool Application::InitRenderers()
{
	if (!RegisterRenderer(new Rendering::DebugRenderer()))
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

	//if (m_RenderTimingManager)
	//	m_RenderTimingManager->SetUpdatesPerSecond(m_MaxFPS);
}

bool Application::RegisterRenderer(Rendering::IRenderer* p_Renderer)
{
	m_Renderers.push_back(p_Renderer);

	return p_Renderer->Init();
}

void Application::OnRender()
{
	if (!m_Window)
		return;

	glfwSwapBuffers(m_Window);
	glfwPollEvents();
}

void Application::OnKeyboard(unsigned char p_Key, int p_X, int p_Y)
{

}

void Application::OnKeyboardUp(unsigned char p_Key, int p_X, int p_Y)
{

}

void Application::OnSpecial(int p_Key, int p_X, int p_Y)
{

}

void Application::OnSpecialUp(int p_Key, int p_X, int p_Y)
{

}

void Application::OnResize(int p_Width, int p_Height)
{

}
