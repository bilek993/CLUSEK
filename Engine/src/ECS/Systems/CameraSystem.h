#pragma once
#include "BaseSystem.h"
#include "../Components/CameraComponent.h"

class CameraSystem final : public BaseSystem
{
public:

	void Start(entt::registry& registry, const ConfigData& configData, Renderer &renderer) override;
	void Update(float deltaTime, entt::registry &registry, IOData& ioData, IODevices &ioDevices,
		Renderer &renderer, RenderWindow &window, ConfigData &configData) override;
};
