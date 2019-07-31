#pragma once
#include "BaseComponentsLoader.h"

class RenderComponentLoader final : public BaseComponentsLoader
{
public:
	void Add(const nlohmann::json& json, entt::registry& registry, const entt::registry::entity_type &entity) override;
};
