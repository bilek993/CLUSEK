#pragma once
#include "BaseComponentsLoader.h"

class CameraVehicleAnimatedTargetComponentLoader final : public BaseComponentsLoader
{
public:
	void Add(nlohmann::json& json, entt::registry& registry, const entt::registry::entity_type &entity) override;
};
