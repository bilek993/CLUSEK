#pragma once
#include "Window/RenderWindow.h"
#include "Loaders/ConfigData.h"
#include "Window/IODevices.h"
#include "Utils/Timer.h"
#include <entt.hpp>
#include "ECS/Systems/BaseSystem.h"
#include "Utils/SystemHolder.h"
#include "GUI/DebugUserInterface.h"
#include "Renderer/PostProcessingSettings.h"

class Engine final
{
public:
	bool Initialize(HINSTANCE hInstance, const ConfigData& configData);
	bool CanUpdate();
	void Update();
private:
	void InitializeScene();
	void CreateSystems();
	void InitializeSystems();
	void InitializeUserInterface();
	void UpdateInputOutputDevices();
	void UpdateSystems(float deltaTime);
	void HandleClosingWithButton() const;

	RenderWindow Window;
	ConfigData Config;

	Timer Time;

	entt::registry Registry;
	std::vector<SystemHolder> Systems;
	int RenderSystemId = 0;

	PostProcessingSettings CurrentPostProcessingSettings{};
	DynamicRenderSettings CurrentRenderSettings{};

	DebugUserInterface DebugUI;

	IODevices InputOutputDevices;
	IOData DataFromIODevices = IOData();
};
