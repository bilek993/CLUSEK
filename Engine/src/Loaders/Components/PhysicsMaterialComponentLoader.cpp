#include "PhysicsMaterialComponentLoader.h"
#include "../../ECS/Components/PhysicsMaterialComponent.h"

void PhysicsMaterialComponentLoader::Add(nlohmann::json& json, entt::registry& registry,
	const entt::registry::entity_type& entity)
{
	auto &component = registry.assign<PhysicsMaterialComponent>(entity);

	MAP_LOADER_BASIC_FIELD(StaticFriction, json, component, float);
	MAP_LOADER_BASIC_FIELD(DynamicFriction, json, component, float);
	MAP_LOADER_BASIC_FIELD(Restitution, json, component, float);
	MAP_LOADER_BASIC_FIELD(SurfaceFilterType, json, component, std::string);
}
