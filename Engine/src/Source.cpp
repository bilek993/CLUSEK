#include <Windows.h>
#include "Utils/SystemInfoUtil.h"
#include "Utils/Logger.h"
#include "Loaders/ConfigLoader.h"
#include "Window/RenderWindow.h"
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "DirectXTK.lib")

// Entry point for the Engine
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance, 
	_In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	Logger::Initialize();
	Logger::Debug("Initialized logger system.");
	SystemInfoUtil::LogInfo();

	auto configuration = ConfigLoader::GetData();

	RenderWindow renderWindow;
	renderWindow.Initialize(hInstance, "test", "test", 800, 600);

	while (renderWindow.ProcessMessages())
	{

	}

	Logger::Debug("Preparing logger to free resources.");
	Logger::Free();

	return 0;
}