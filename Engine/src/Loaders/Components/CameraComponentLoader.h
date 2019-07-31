#pragma once
#include "BaseComponentsLoader.h"

class CameraComponentLoader final : public BaseComponentsLoader
{
public:
	void Add(const nlohmann::json& json, entt::registry& registry, const entt::registry::entity_type &entity) override;
};
