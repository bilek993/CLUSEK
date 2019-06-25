#include <Windows.h>
#include <string>
#include "Utils/Logger.h"
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "DirectXTK.lib")

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance, 
	_In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	Logger::Initialize();
	Logger::Debug("Initialized logger system.");

	while(true)
	{
		Sleep(500);
	}

	Logger::Free();

	return 0;
}