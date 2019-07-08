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
									Config.MultisamplesQuality,
									Config.MainCameraFov,
									Config.MainCameraNearZ,
									Config.MainCameraFarZ))
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
	const auto deltaTime = Time.GetDeltaTimeAndRestart();

	UpdateInputOutputDevices();
	HandleClosingWithButton();

	// START OF TMP CODE
	// Remove this as fast as possible and replace with proper ECS handling
	auto camera = GraphicsRenderer.GetPointerToCamera();

	if (DataFromIODevices.MouseState.rightButton)
	{
		InputOutputDevices.ChangeMouseToRelativeMode(Window.GetHandle());
		camera->AdjustRotation(static_cast<float>(DataFromIODevices.MouseState.y) * 0.001f * deltaTime, 
			static_cast<float>(DataFromIODevices.MouseState.x) * 0.001f * deltaTime, 0.0f);
	}
	else
	{
		InputOutputDevices.ChangeMouseToAbsoluteMode(Window.GetHandle());
	}

	auto cameraSpeed = 0.0025f * deltaTime;
	if (DataFromIODevices.KeyboardState.LeftShift || DataFromIODevices.KeyboardState.RightShift)
		cameraSpeed *= 5;

	if (DataFromIODevices.KeyboardState.W)
	{
		camera->AdjustPosition(DirectX::XMVectorScale(camera->GetForwardVector(), cameraSpeed));
	}
	if (DataFromIODevices.KeyboardState.A)
	{
		camera->AdjustPosition(DirectX::XMVectorScale(camera->GetRightVector(), -cameraSpeed));
	}
	if (DataFromIODevices.KeyboardState.S)
	{
		camera->AdjustPosition(DirectX::XMVectorScale(camera->GetForwardVector(), -cameraSpeed));
	}
	if (DataFromIODevices.KeyboardState.D)
	{
		camera->AdjustPosition(DirectX::XMVectorScale(camera->GetRightVector(), cameraSpeed));
	}
	// END OF TMP CODE
}

void Engine::Render()
{
	GraphicsRenderer.RenderFrame();
}

void Engine::UpdateInputOutputDevices()
{
	InputOutputDevices.Update();
	DataFromIODevices = InputOutputDevices.Get();
}

void Engine::HandleClosingWithButton()
{
	if (DataFromIODevices.KeyboardState.Escape)
	{
		Logger::Debug("Escape key pressed. Sending quit message...");
		Window.UserRequestedQuit();
	}
}
