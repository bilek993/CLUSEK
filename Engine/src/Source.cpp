#include <Windows.h>
#include "Utils/SystemInfoUtil.h"
#include "Utils/Logger.h"
#include "Engine.h"
#include "Loaders/ConfigLoader.h"
#include "Utils/DebuggerDetection.h"

// Entry point for the Engine
int APIENTRY wWinMain(	_In_ HINSTANCE hInstance,
						_In_opt_ HINSTANCE hPrevInstance, 
						_In_ LPWSTR lpCmdLine, 
						_In_ int nCmdShow)
{
	if (DebuggerDetection::Check())
	{
		MessageBox(	nullptr, 
					"Due to the debugger, the CLUSEK game engine has to be terminated. Please disable all debugger tools and try again.", 
					"Debugger detected!", 
					MB_ICONEXCLAMATION | MB_OK);
		return 2;
	}
	
	const auto configData = ConfigLoader::GetData();

	Logger::Initialize(configData.LoggerEnabled, configData.LoggerDestination);
	Logger::SetLevel(configData.LoggerLevel);
	Logger::Debug("Initialized logger system.");
	SystemInfoUtil::LogInfo();

	Engine engine;
	engine.Initialize(hInstance, configData);

	while (engine.CanUpdate())
	{
		engine.Update();
	}

	Logger::Debug("Preparing logger to free resources.");
	Logger::Free();

	return 0;
}