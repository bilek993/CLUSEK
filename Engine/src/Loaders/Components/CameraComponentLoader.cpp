#include "CameraComponentLoader.h"
#include "../../ECS/Components/CameraComponent.h"

void CameraComponentLoader::Add(nlohmann::json& json, entt::registry& registry,
	const entt::registry::entity_type& entity)
{
	registry.assign<CameraComponent>(entity);
}
