#pragma once
#include "BaseSystem.h"
#include "../Components/CameraComponent.h"

class CameraSystem final : public BaseSystem
{
public:

	void Start(entt::registry& registry, const RenderWindow &window, const ConfigData& configData) override;
	void Update(float deltaTime, entt::registry &registry, IOData& ioData, IODevices &ioDevices,
		RenderWindow &window, ConfigData &configData) override;
};
