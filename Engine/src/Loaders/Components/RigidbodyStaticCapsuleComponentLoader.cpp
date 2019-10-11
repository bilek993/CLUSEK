#include "RigidbodyStaticCapsuleComponentLoader.h"
#include "../../ECS/Components/RigidbodyStaticCapsuleComponent.h"

void RigidbodyStaticCapsuleComponentLoader::Add(nlohmann::json& json, entt::registry& registry,
	const entt::registry::entity_type& entity)
{
	auto &component = registry.assign<RigidbodyStaticCapsuleComponent>(entity);

	MAP_LOADER_BASIC_FIELD(Height, json, component, float);
	MAP_LOADER_BASIC_FIELD(Radius, json, component, float);
}
