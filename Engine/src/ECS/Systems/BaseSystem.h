#pragma once
#include <entt.hpp>
#include "../../Window/IODevices.h"
#include "../../Loaders/ConfigData.h"
#include "../../Window/RenderWindow.h"
#include "../../Renderer/DynamicRenderSettings.h"
#include "../../Renderer/PostProcessingSettings.h"
#include "../../Utils/Timer.h"

class BaseSystem
{
public:
	virtual ~BaseSystem() = default;

	void Initialize(entt::registry *registry, RenderWindow *window, ConfigData *configData,
		DynamicRenderSettings *renderSettings, IOData *ioData, IODevices *ioDevices, PostProcessingSettings *postProcessingSettings);

	virtual void Start() = 0;

	void BeforeUpdate();
	virtual void Update(float deltaTime) = 0;
	void AfterUpdate();

	float GetDeltaTime() const;
protected:
	entt::registry *Registry = nullptr;
	RenderWindow *Window = nullptr;
	ConfigData *ConfigurationData = nullptr;
	DynamicRenderSettings *CurrentRenderSettings = nullptr;
	IOData *InputOutputData = nullptr;
	IODevices *InputOutputDevices = nullptr;
	PostProcessingSettings *CurrentPostProcessingSettings = nullptr;

private:
	Timer PerformanceTimer{};
	float SystemDeltaTime = 0;
};
