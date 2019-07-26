#pragma once
#include <entt.hpp>
#include "../../Window/IODevices.h"
#include "../../Loaders/ConfigData.h"
#include "../../Window/RenderWindow.h"
#include "../../Renderer/LightSettings.h"

class BaseSystem
{
public:
	virtual ~BaseSystem() = default;
	virtual void Start(entt::registry &registry, const RenderWindow &window, const ConfigData &configData,
		LightSettings &lightSettings) = 0;
	virtual void Update(float deltaTime, entt::registry &registry, IOData& ioData, IODevices &ioDevices, 
		RenderWindow &window, ConfigData &configData, LightSettings &lightSettings) = 0;
};
