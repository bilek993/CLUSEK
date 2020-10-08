#pragma once
#include "BaseComponentsLoader.h"

class RigidbodyStaticConvexComponentLoader final : public BaseComponentsLoader
{
public:
	void Add(nlohmann::json& json, entt::registry& registry, const entt::registry::entity_type &entity) override;
};
