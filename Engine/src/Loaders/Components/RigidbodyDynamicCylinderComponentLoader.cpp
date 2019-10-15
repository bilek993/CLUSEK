#include "RigidbodyDynamicCylinderComponentLoader.h"
#include "../../ECS/Components/RigidbodyDynamicCylinderComponent.h"

void RigidbodyDynamicCylinderComponentLoader::Add(nlohmann::json& json, entt::registry& registry,
	const entt::registry::entity_type& entity)
{
	auto &component = registry.assign<RigidbodyDynamicCylinderComponent>(entity);

	MAP_LOADER_BASIC_FIELD(Width, json, component, float);
	MAP_LOADER_BASIC_FIELD(Radius, json, component, float);
	MAP_LOADER_BASIC_FIELD(Density, json, component, float);
}
