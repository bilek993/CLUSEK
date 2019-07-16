#include "Engine.h"
#include "Utils/Logger.h"
#include "ECS/Components/CameraComponent.h"
#include "ECS/Systems/CameraSystem.h"
#include "ECS/Systems/RenderSystem.h"
#include "ECS/Components/RenderComponent.h"
#include "ECS/Components/TransformComponent.h"
#include "ECS/Systems/RotationSystem.h"

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
	const auto renderSystem = dynamic_cast<RenderSystem*>(Systems[RenderSystemId].second.get());

	UpdateInputOutputDevices();
	renderSystem->RenderFrameBegin();
	UpdateSystems(deltaTime);
	renderSystem->RenderFrameEnd();
	HandleClosingWithButton();
}

void Engine::InitializeScene()
{
	const auto cameraEntity = Registry.create();
	Registry.assign<CameraComponent>(cameraEntity);
	Registry.assign<TransformComponent>(cameraEntity);

	const auto sampleObjectEntity = Registry.create();
	Registry.assign<RenderComponent>(sampleObjectEntity);
	Registry.assign<TransformComponent>(sampleObjectEntity);
}

void Engine::CreateSystems()
{
	Systems.emplace_back(std::make_pair("CameraSystem", std::make_unique<CameraSystem>()));
	Systems.emplace_back(std::make_pair("RotationSystem", std::make_unique<RotationSystem>()));
	Systems.emplace_back(std::make_pair("RenderSystem", std::make_unique<RenderSystem>()));

	for (auto i = 0; i < Systems.size(); i++)
	{
		if (Systems[i].first == "RenderSystem")
		{
			RenderSystemId = i;
			return;
		}
	}

	Logger::Error("Render system not registered as system!");
}

void Engine::InitializeSystems()
{
	for (auto& system : Systems)
	{
		system.second->Start(Registry, Window, Config);
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
		system.second->Update(	deltaTime,
								Registry,
								DataFromIODevices,
								InputOutputDevices,
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
