#include "TransformComponentLoader.h"
#include "../../ECS/Components/TransformComponent.h"

void TransformComponentLoader::Add(const nlohmann::json& json, entt::registry& registry,
	const entt::registry::entity_type& entity)
{
	registry.assign<TransformComponent>(entity);
}
