#include "RigidbodyDynamicCapsuleComponentLoader.h"
#include "../../ECS/Components/RigidbodyDynamicCapsuleComponent.h"

void RigidbodyDynamicCapsuleComponentLoader::Add(nlohmann::json& json, entt::registry& registry,
	const entt::registry::entity_type& entity)
{
	auto &component = registry.assign<RigidbodyDynamicCapsuleComponent>(entity);

	MAP_LOADER_BASIC_FIELD(Height, json, component, float);
	MAP_LOADER_BASIC_FIELD(Radius, json, component, float);
	MAP_LOADER_BASIC_FIELD(Density, json, component, float);
}
