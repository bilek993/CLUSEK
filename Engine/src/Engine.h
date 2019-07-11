#pragma once
#include "Window/RenderWindow.h"
#include "Loaders/ConfigData.h"
#include "Window/IODevices.h"
#include "Renderer/Renderer.h"
#include "Timer.h"
#include <entt.hpp>
#include "ECS/Systems/BaseSystem.h"

class Engine final
{
public:
	bool Initialize(HINSTANCE hInstance, const ConfigData configData);
	bool CanUpdate();
	void Update();
private:
	void InitializeScene();
	void CreateSystems();
	void InitializeSystems();
	void UpdateInputOutputDevices();
	void UpdateSystems(float deltaTime);
	void HandleClosingWithButton();

	RenderWindow Window;
	ConfigData Config;

	Renderer GraphicsRenderer;

	Timer Time;

	entt::registry Registry;
	std::vector<std::unique_ptr<BaseSystem>> Systems;

	IODevices InputOutputDevices;
	IOData DataFromIODevices = IOData();
};
