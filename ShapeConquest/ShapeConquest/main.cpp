#include <iostream>
#include "Graphics Pipeline/APIDeffinition.h"
#include "GameWindow.h"
#include <ctime>

int WINAPI wWinMain(_In_ HINSTANCE const hInstance, _In_opt_ HINSTANCE const hPrevInstance, _In_ const LPWSTR const lpCmdLine, _In_ const int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	//_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );

	if (!AllocConsole())
		MessageBox(nullptr, "The console window was not created", nullptr, MB_ICONEXCLAMATION);
	FILE* fp;
	const auto console = GetConsoleWindow();

	MoveWindow(console, 0, 0, 420, 500, TRUE);
	freopen_s(&fp, "CONOUT$", "w", stdout);
	freopen_s(&fp, "CONIN$", "r", stdin);
#ifdef DIRECT_X
	std::cout << "DirectX" << std::endl;
#endif
	
	std::cout << "Shape Conquest" << std::endl;
	std::unique_ptr<GameWindow> gameWindow = std::make_unique<GameWindow>();
	gameWindow->Run();
	//_CrtDumpMemoryLeaks();
	return -1;
}
