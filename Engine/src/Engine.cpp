#include "Engine.h"
#include "Utils/Logger.h"

bool Engine::Initialize(const HINSTANCE hInstance, const ConfigData configData)
{
	Logger::Debug("Engine initialization...");

	const auto hr = CoInitialize(nullptr);
	if (FAILED(hr))
	{
		Logger::Error("Error when initializing COM libs.");
		return false;
	}

	Config = configData;

	if (!Window.Initialize(hInstance, Config.WindowTitle, Config.WindowClassName, Config.WindowWidth, Config.WindowHeight))
		return false;

	if (GraphicsRenderer.Initialize(Window.GetHandle(),
									Config.WindowWidth,
									Config.WindowHeight,
									Config.Fullscreen,
									Config.SyncIntervals,
									Config.SelectedAdapterId,
									Config.RefreshRateNumerator,
									Config.RefreshRateDenominator,
									Config.MultisamplesCount,
									Config.MultisamplesQuality))
	{
		return false;
	}

	return true;
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

void Engine::Render() const
{
	GraphicsRenderer.RenderFrame();
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
