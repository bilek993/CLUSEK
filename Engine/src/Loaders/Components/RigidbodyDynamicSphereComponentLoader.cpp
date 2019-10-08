#include "RigidbodyDynamicSphereComponentLoader.h"
#include "../../ECS/Components/RigidbodyDynamicSphereComponent.h"

void RigidbodyDynamicSphereComponentLoader::Add(nlohmann::json& json, entt::registry& registry,
	const entt::registry::entity_type& entity)
{
	auto &component = registry.assign<RigidbodyDynamicSphereComponent>(entity);

	MAP_LOADER_BASIC_FIELD(Radius, json, component, float);
	MAP_LOADER_BASIC_FIELD(Density, json, component, float);
}
