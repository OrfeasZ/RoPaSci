#include <Application.h>

int WINAPI WinMain(HINSTANCE p_Instance, HINSTANCE p_PrevInstance, LPSTR p_CmdLine, int p_ShowCmd)
{
	Application::GetInstance()->Init(500, 500, 800, 600, "RoPaSci II");
	return 0;
}