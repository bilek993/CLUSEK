#include "Engine.h"
#include "Loaders/ConfigLoader.h"
#include "Utils/Logger.h"

bool Engine::Initialize(HINSTANCE hInstance)
{
	Logger::Debug("Engine initialization...");
	Config = ConfigLoader::GetData();

	const auto windowInitSuccess = Window.Initialize(hInstance, Config.WindowTitle, Config.WindowClassName, Config.WindowWidth, Config.WindowHeight);
	if (windowInitSuccess)
		InputOutputDevices.ChangeMouseToRelativeMode(Window.GetHandle());

	return windowInitSuccess;
}

bool Engine::CanUpdate()
{
	return Window.ProcessMessages();
}

void Engine::Update()
{
	InputOutputDevices.Update();
	auto ioDevicesData = InputOutputDevices.Get();
}
