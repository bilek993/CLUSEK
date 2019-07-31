#pragma once
#include <entt.hpp>
#include <json.hpp>

#define MAP_LOADER_BASIC_FIELD(FIELD, JSON, COMPONENT, TYPE) if (!JSON[#FIELD].is_null()) COMPONENT.FIELD = JSON[#FIELD].get<TYPE>();

class BaseComponentsLoader
{
public:
	virtual ~BaseComponentsLoader() = default;
	void virtual Add(const nlohmann::json& json, entt::registry& registry, const entt::registry::entity_type &entity) = 0;
};