#include "RigidbodyStaticPlaneComponentLoader.h"
#include "../../ECS/Components/RigidbodyStaticPlaneComponent.h"

void RigidbodyStaticPlaneComponentLoader::Add(nlohmann::json& json, entt::registry& registry,
	const entt::registry::entity_type& entity)
{
	auto &component = registry.assign<RigidbodyStaticPlaneComponent>(entity);

	MAP_LOADER_BASIC_FIELD(NormalX, json, component, float);
	MAP_LOADER_BASIC_FIELD(NormalY, json, component, float);
	MAP_LOADER_BASIC_FIELD(NormalZ, json, component, float);
	MAP_LOADER_BASIC_FIELD(Distance, json, component, float);
}
