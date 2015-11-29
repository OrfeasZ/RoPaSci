#include <Application.h>

#ifdef _WIN32
int WINAPI WinMain(HINSTANCE p_Instance, HINSTANCE p_PrevInstance, LPSTR p_CmdLine, int p_ShowCmd)
{
	Application::GetInstance()->Init(1024, 768, "RoPaSci II");
	return 0;
}
#else
int main(int p_Argc, char** p_Argv)
{
	Application::GetInstance()->Init(1024, 768, "RoPaSci II");
	return 0;
}
#endif