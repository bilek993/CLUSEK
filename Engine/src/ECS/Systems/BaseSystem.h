#pragma once
#include <entt.hpp>
#include "../../Window/IODevices.h"
#include "../../Loaders/ConfigData.h"
#include "../../Window/RenderWindow.h"
#include "../../Renderer/DynamicRenderSettings.h"

class BaseSystem
{
public:
	virtual ~BaseSystem() = default;

	void Initialize(entt::registry *registry, RenderWindow *window, ConfigData *configData,
		DynamicRenderSettings *renderSettings, IOData *ioData, IODevices *ioDevices);

	virtual void Start() = 0;
	virtual void Update(float deltaTime) = 0;
protected:
	entt::registry *Registry = nullptr;
	RenderWindow *Window = nullptr;
	ConfigData *ConfigurationData = nullptr;
	DynamicRenderSettings *CurrentRenderSettings = nullptr;
	IOData *InputOutputData = nullptr;
	IODevices *InputOutputDevices = nullptr;
};
