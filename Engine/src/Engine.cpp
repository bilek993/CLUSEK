#include "Engine.h"
#include "Loaders/ConfigLoader.h"
#include "Utils/Logger.h"

bool Engine::Initialize(HINSTANCE hInstance)
{
	Logger::Debug("Engine initialization...");
	Config = ConfigLoader::GetData();
	return Window.Initialize(hInstance, Config.WindowTitle, Config.WindowClassName, Config.WindowWidth, Config.WindowHeight);
}

bool Engine::CanUpdate()
{
	return Window.ProcessMessages();
}

void Engine::Update()
{
	InputOutputDevices.Update();
	auto ioDevicesData = InputOutputDevices.Get();

	if (ioDevicesData.MouseState.leftButton && ioDevicesData.KeyboardState.LeftAlt)
	{
		InputOutputDevices.ChangeMouseToRelativeMode(Window.GetHandle());
	}
	else
	{
		InputOutputDevices.ChangeMouseToAbsoluteMode(Window.GetHandle());
	}

	Logger::Debug(std::to_string(ioDevicesData.MouseState.x));
}
