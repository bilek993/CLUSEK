#include "WheelComponentLoader.h"
#include "../../ECS/Components/WheelComponent.h"

void WheelComponentLoader::Add(nlohmann::json& json, entt::registry& registry,
	const entt::registry::entity_type& entity)
{
	auto &component = registry.assign<WheelComponent>(entity);
}
