#include "CameraComponentLoader.h"
#include "../../ECS/Components/CameraComponent.h"

void CameraComponentLoader::Add(nlohmann::json& json, entt::registry& registry,
	const entt::registry::entity_type& entity)
{
	auto &component = registry.assign<CameraComponent>(entity);

	MAP_LOADER_BASIC_FIELD(UseTarget, json, component, bool);
}
