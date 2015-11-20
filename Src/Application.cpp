#include "Application.h"

#include <Util/Utils.h>
#include <VFS/FileSystem.h>

#include <Managers/ShaderManager.h>
#include <Managers/TimingManager.h>
#include <Managers/SimulationManager.h>

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
	m_ShaderManager(0),
	m_CurrentState(Application::Windowed),
	m_TickRate(120),
	m_MaxFPS(300)
{

}

Application::~Application()
{
	if (m_SimulationManager)
		delete m_SimulationManager;

	if (m_RenderTimingManager)
		delete m_RenderTimingManager;

	if (m_ShaderManager)
		delete m_ShaderManager;

	m_SimulationManager = nullptr;
	m_RenderTimingManager = nullptr;
	m_ShaderManager = nullptr;

	for (auto s_Renderer : m_Renderers)
		delete s_Renderer;

	m_Renderers.clear();
}

void Application::Init(int p_WindowX, int p_WindowY, int p_WindowWidth, int p_WindowHeight, const std::string& p_WindowTitle)
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

	// Initialize OpenGL and GLUT.
	glutInit(&__argc, __argv);

	// Setup our window.
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(p_WindowX, p_WindowY);
	glutInitWindowSize(p_WindowWidth, p_WindowHeight);
	glutCreateWindow(p_WindowTitle.c_str());

	// Register the GLUT callbacks.
	glutDisplayFunc(OnRenderStatic);
	glutKeyboardFunc(OnKeyboardStatic);
	glutKeyboardUpFunc(OnKeyboardUpStatic);
	glutSpecialFunc(OnSpecialStatic);
	glutSpecialUpFunc(OnSpecialUpStatic);
	glutReshapeFunc(OnResizeStatic);
	glutIdleFunc(OnIdleStatic);

	// Initialize glew.
	glewInit();

	// Enable the features we want to use.
	glEnable(GL_DEPTH_TEST);

	// Initialize our managers.
	if (!InitManagers())
	{
		Shutdown();
		return;
	}

	// Set required OpenGL options.
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	Logger(Util::LogLevel::Info, "Initializing renderer...");

	// Initialize our renderers.
	if (!InitRenderers())
	{
		Shutdown();
		return;
	}

	// Start the simulation.
	m_SimulationManager->Init();

	// Start the main application loop.
	glutMainLoop();

	// After we're done we should shut down.
	Shutdown();
}

void Application::Shutdown()
{
	DestroyInstance();
}

bool Application::InitManagers()
{
	m_ShaderManager = new Managers::ShaderManager();

	if (m_ShaderManager->CreateShaderProgram("Test", Managers::ShaderManager::VertexShader | Managers::ShaderManager::FragmentShader) == 0)
		return false;

	m_RenderTimingManager = new	Managers::TimingManager(m_MaxFPS);
	m_SimulationManager = new Managers::SimulationManager(m_TickRate);

	return true;
}

bool Application::InitRenderers()
{
	RegisterRenderer(new Rendering::DebugRenderer());

	return true;
}

void Application::SetTickRate(int p_TickRate)
{
	m_TickRate = p_TickRate;

	if (m_SimulationManager)
		m_SimulationManager->SetTickRate(m_TickRate);
}

void Application::SetMaxFPS(int p_MaxFPS)
{
	m_MaxFPS = p_MaxFPS;

	if (m_RenderTimingManager)
		m_RenderTimingManager->SetUpdatesPerSecond(m_MaxFPS);
}

void Application::RegisterRenderer(Rendering::IRenderer* p_Renderer)
{
	m_Renderers.push_back(p_Renderer);

	if (m_SimulationManager)
		m_SimulationManager->RegisterRenderer(p_Renderer);
}

void Application::OnRenderStatic()
{
	GetInstance()->OnRender();
}

void Application::OnKeyboardStatic(unsigned char p_Key, int p_X, int p_Y)
{
	GetInstance()->OnKeyboard(p_Key, p_X, p_Y);
}

void Application::OnKeyboardUpStatic(unsigned char p_Key, int p_X, int p_Y)
{
	GetInstance()->OnKeyboardUp(p_Key, p_X, p_Y);
}

void Application::OnSpecialStatic(int p_Key, int p_X, int p_Y)
{
	GetInstance()->OnSpecial(p_Key, p_X, p_Y);
}

void Application::OnSpecialUpStatic(int p_Key, int p_X, int p_Y)
{
	GetInstance()->OnSpecialUp(p_Key, p_X, p_Y);
}

void Application::OnResizeStatic(int p_Width, int p_Height)
{
	GetInstance()->OnResize(p_Width, p_Height);
}

void Application::OnIdleStatic()
{
	GetInstance()->OnIdle();
}

void Application::OnRender()
{
	if (!m_RenderTimingManager)
		return;

	m_RenderTimingManager->Update();
	double s_Delta = m_RenderTimingManager->GetLastDelta();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.f, 0.3f, 0.3f, 1.f);

	for (auto s_Program : m_ShaderManager->GetShaderPrograms())
		glUseProgram(s_Program.second);

	glDrawArrays(GL_TRIANGLES, 0, 3);

	for (auto s_Renderer : m_Renderers)
		s_Renderer->Render(s_Delta);

	glutSwapBuffers();
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

void Application::OnIdle()
{
	glutPostRedisplay();
}
