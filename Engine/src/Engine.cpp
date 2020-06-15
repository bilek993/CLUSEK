#include "Engine.h"
#include "Utils/Logger.h"
#include "ECS/Systems/CameraSystem.h"
#include "ECS/Systems/RenderSystem.h"
#include "ECS/Components/ModelRenderComponent.h"
#include "Loaders/MapLoader.h"
#include "ECS/Systems/SkyboxSystem.h"
#include "ECS/Systems/PhysicsSystem.h"
#include "ECS/Systems/TransformSystem.h"
#include "ECS/Systems/VehiclePlayerControllerSystem.h"

bool Engine::Initialize(const HINSTANCE hInstance, const ConfigData& configData)
{
	Logger::Debug("Engine initialization...");

	Timer engineInitializationTimer;
	engineInitializationTimer.Restart();

	HRESULT hr;
	if (configData.ComInitializationMultithreaded)
		hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	else
		hr = CoInitialize(nullptr);

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
	InitializeUserInterface();

	Logger::Debug("Resetting timer...");
	Time.Restart();

	const auto engineInitializationTime = static_cast<int>(ceil(engineInitializationTimer.GetDeltaTimeAndRestart()));
	Logger::Debug("Engine initialization done!");
	Logger::Debug("Initialization took: " + std::to_string(engineInitializationTime) + " ms.");

	return true;
}

bool Engine::CanUpdate()
{
	return Window.ProcessMessages();
}

void Engine::Update()
{
	const auto deltaTime = Time.GetDeltaTimeAndRestart();
	const auto renderSystem = dynamic_cast<RenderSystem*>(Systems[RenderSystemId].System.get());

	UpdateInputOutputDevices();
	renderSystem->RenderFrameBegin();
	UpdateSystems(deltaTime);

	DebugUI.Update(	deltaTime, 
					&Config, 
					&DataFromIODevices, 
					&Systems, 
					RenderSystemId,
					PhysicsSystemId,
					&CurrentRenderSettings, 
					&CurrentPostProcessingSettings, 
					&Registry);

	renderSystem->RenderFrameEnd();
	HandleClosingWithButton();
}

void Engine::InitializeScene()
{
	MapLoader::CreateEntitiesFromMapFile(Config.PathToMap, Registry);
}

void Engine::CreateSystems()
{
	// Render system should be first
	RegisterNewSystem<RenderSystem>("Render System");

	// Transform system should be second
	RegisterNewSystem<TransformSystem>("Transform System");

	RegisterNewSystem<PhysicsSystem>("Physics System");
	RegisterNewSystem<CameraSystem>("Camera System");
	RegisterNewSystem<SkyboxSystem>("Skybox System");
	RegisterNewSystem<VehiclePlayerControllerSystem>("Vehicle Player Controller System");

	for (auto i = 0; i < Systems.size(); i++)
	{
		if (Systems[i].Name == "Render System")
			RenderSystemId = i;
		else if (Systems[i].Name == "Physics System")
			PhysicsSystemId = i;
	}
}

void Engine::InitializeSystems()
{
	for (auto& system : Systems)
	{
		system.System->Initialize(	&Registry, 
									&Window,
									&Config, 
									&CurrentRenderSettings, 
									&DataFromIODevices,
									&InputOutputDevices, 
									&CurrentPostProcessingSettings);
		system.System->Start();
	}
}

void Engine::InitializeUserInterface()
{
	const auto renderSystem = dynamic_cast<RenderSystem*>(Systems[RenderSystemId].System.get());
	DebugUI.Initialize(Window.GetHandle(), renderSystem->GetPointerToDevice(), renderSystem->GetPointerToDeviceContext(), &Config, [this]()
	{
		Window.UserRequestedQuit();
	});
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
		if (!system.Enabled)
			continue;

		system.System->BeforeUpdate();
		system.System->Update(deltaTime);
		system.System->AfterUpdate();
	}
}

void Engine::HandleClosingWithButton() const
{
	if (DataFromIODevices.KeyboardState.Escape)
	{
		Logger::Debug("Escape key pressed. Sending quit message...");
		Window.UserRequestedQuit();
	}
	if (DataFromIODevices.GamePadState.IsConnected() && DataFromIODevices.GamePadState.IsBackPressed())
	{
		Logger::Debug("Back button pressed. Sending quit message...");
		Window.UserRequestedQuit();
	}
}
