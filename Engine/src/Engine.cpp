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
	UpdateInputOutputDevices();
	HandleBasicInputOutput();
}

void Engine::UpdateInputOutputDevices()
{
	InputOutputDevices.Update();
	DataFromIODevices = InputOutputDevices.Get();
}

void Engine::HandleBasicInputOutput()
{
	if (DataFromIODevices.MouseState.leftButton && DataFromIODevices.KeyboardState.LeftAlt)
	{
		InputOutputDevices.ChangeMouseToRelativeMode(Window.GetHandle());
	}
	else
	{
		InputOutputDevices.ChangeMouseToAbsoluteMode(Window.GetHandle());
	}

	if (DataFromIODevices.KeyboardState.Escape)
	{
		Logger::Debug("Escape key pressed. Sending quit message...");
		Window.UserRequestedQuit();
	}
}
