#include "PhysicsMaterialComponentLoader.h"
#include "../../ECS/Components/PhysicsMaterialComponent.h"

void PhysicsMaterialComponentLoader::Add(nlohmann::json& json, entt::registry& registry,
	const entt::registry::entity_type& entity)
{
	auto &component = registry.assign<PhysicsMaterialComponent>(entity);

	MAP_LOADER_BASIC_FIELD(Name, json, component, std::string);
	MAP_LOADER_BASIC_FIELD(SurfaceFilterType, json, component, std::string);
}
