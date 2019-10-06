#include "RigidbodyStaticBoxComponentLoader.h"
#include "../../ECS/Components/RigidbodyStaticBoxComponent.h"

void RigidbodyStaticBoxComponentLoader::Add(nlohmann::json& json, entt::registry& registry,
	const entt::registry::entity_type& entity)
{
	auto &component = registry.assign<RigidbodyStaticBoxComponent>(entity);

	MAP_LOADER_BASIC_FIELD(Width, json, component, float);
	MAP_LOADER_BASIC_FIELD(Height, json, component, float);
	MAP_LOADER_BASIC_FIELD(Depth, json, component, float);
}
