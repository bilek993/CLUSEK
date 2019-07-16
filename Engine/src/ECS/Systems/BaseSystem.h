#pragma once
#include <entt.hpp>
#include "../../Window/IODevices.h"
#include "../../Loaders/ConfigData.h"
#include "../../Window/RenderWindow.h"

class BaseSystem
{
public:
	virtual ~BaseSystem() = default;
	virtual void Start(entt::registry &registry, const RenderWindow &window, const ConfigData &configData) = 0;
	virtual void Update(float deltaTime, entt::registry &registry, IOData& ioData, IODevices &ioDevices, 
		RenderWindow &window, ConfigData &configData) = 0;
};
