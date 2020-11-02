#include "RoadComponentLoader.h"
#include "../../ECS/Components/RoadComponent.h"

void RoadComponentLoader::Add(nlohmann::json& json, entt::registry& registry, const entt::registry::entity_type& entity)
{
	auto &component = registry.assign<RoadComponent>(entity);

	// TODO: Add implementation
}
