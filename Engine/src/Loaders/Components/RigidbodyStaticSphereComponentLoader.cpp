#include "RigidbodyStaticSphereComponentLoader.h"
#include "../../ECS/Components/RigidbodyStaticSphereComponent.h"

void RigidbodyStaticSphereComponentLoader::Add(nlohmann::json& json, entt::registry& registry,
	const entt::registry::entity_type& entity)
{
	auto &component = registry.assign<RigidbodyStaticSphereComponent>(entity);

	MAP_LOADER_BASIC_FIELD(Radius, json, component, float);
}
