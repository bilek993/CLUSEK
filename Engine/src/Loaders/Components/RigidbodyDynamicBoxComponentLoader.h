#pragma once
#include "BaseComponentsLoader.h"

class RigidbodyDynamicBoxComponentLoader final : public BaseComponentsLoader
{
public:
	void Add(nlohmann::json& json, entt::registry& registry, const entt::registry::entity_type &entity) override;
};
