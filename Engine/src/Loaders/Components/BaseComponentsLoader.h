#pragma once
#include <entt.hpp>
#include <json.hpp>

class BaseComponentsLoader
{
public:
	virtual ~BaseComponentsLoader() = default;
	void virtual Add(const nlohmann::json& json, entt::registry& registry, const entt::registry::entity_type &entity) = 0;
};