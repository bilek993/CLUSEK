#include "RigidbodyStaticCylinderComponentLoader.h"
#include "RigidbodyStaticCylinderComponent.h"

void RigidbodyStaticCylinderComponentLoader::Add(nlohmann::json& json, entt::registry& registry,
	const entt::registry::entity_type& entity)
{
	auto &component = registry.assign<RigidbodyStaticCylinderComponent>(entity);

	MAP_LOADER_BASIC_FIELD(Width, json, component, float);
	MAP_LOADER_BASIC_FIELD(Radius, json, component, float);
}
