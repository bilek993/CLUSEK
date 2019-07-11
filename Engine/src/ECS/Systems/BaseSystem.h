#pragma once
#include <entt.hpp>
#include "../../Window/IODevices.h"
#include "../../Renderer/Renderer.h"
#include "../../Loaders/ConfigData.h"
#include "../../Window/RenderWindow.h"

class BaseSystem
{
public:
	virtual ~BaseSystem() = default;
	virtual void Start(entt::registry &registry, const ConfigData &configData, Renderer &renderer) = 0;
	virtual void Update(float deltaTime, entt::registry &registry, IOData& ioData, IODevices &ioDevices, 
		Renderer &renderer, RenderWindow &window, ConfigData &configData) = 0;
};
