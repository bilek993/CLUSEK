#include "RigidbodyDynamicBoxComponentLoader.h"
#include "../../ECS/Components/RigidbodyDynamicBoxComponent.h"

void RigidbodyDynamicBoxComponentLoader::Add(nlohmann::json& json, entt::registry& registry,
	const entt::registry::entity_type& entity)
{
	auto &component = registry.assign<RigidbodyDynamicBoxComponent>(entity);

	MAP_LOADER_BASIC_FIELD(Width, json, component, float);
	MAP_LOADER_BASIC_FIELD(Height, json, component, float);
	MAP_LOADER_BASIC_FIELD(Depth, json, component, float);
	MAP_LOADER_BASIC_FIELD(Density, json, component, float);
}
