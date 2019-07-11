#include "Engine.h"
#include "Utils/Logger.h"
#include "ECS/Components/CameraComponent.h"
#include "ECS/Systems/CameraSystem.h"
#include "ECS/Systems/RenderSystem.h"
#include "ECS/Components/RenderComponent.h"

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

	if (!GraphicsRenderer.Initialize(Window.GetHandle(),
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

	InitializeScene();
	CreateSystems();
	InitializeSystems();

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
	UpdateSystems(deltaTime);
	HandleClosingWithButton();
}

void Engine::InitializeScene()
{
	const auto cameraEntity = Registry.create();
	Registry.assign<CameraComponent>(cameraEntity);

	const auto sampleObjectEntity = Registry.create();
	Registry.assign<RenderComponent>(sampleObjectEntity);
}

void Engine::CreateSystems()
{
	Systems.emplace_back(std::make_unique<CameraSystem>());
	Systems.emplace_back(std::make_unique<RenderSystem>());
}

void Engine::InitializeSystems()
{
	for (auto& system : Systems)
	{
		system->Start(Registry, Config, GraphicsRenderer);
	}
}

void Engine::UpdateInputOutputDevices()
{
	InputOutputDevices.Update();
	DataFromIODevices = InputOutputDevices.Get();
}

void Engine::UpdateSystems(const float deltaTime)
{
	for (auto& system : Systems)
	{
		system->Update(	deltaTime,
						Registry,
						DataFromIODevices,
						InputOutputDevices,
						GraphicsRenderer,
						Window,
						Config);
	}
}

void Engine::HandleClosingWithButton()
{
	if (DataFromIODevices.KeyboardState.Escape)
	{
		Logger::Debug("Escape key pressed. Sending quit message...");
		Window.UserRequestedQuit();
	}
}
