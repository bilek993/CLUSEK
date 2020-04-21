#include "CameraTargetComponentLoader.h"
#include "../../ECS/Components/CameraTargetComponent.h"

void CameraTargetComponentLoader::Add(nlohmann::json& json, entt::registry& registry,
	const entt::registry::entity_type& entity)
{
	auto &component = registry.assign<CameraTargetComponent>(entity);

	MAP_LOADER_BASIC_FIELD(Active, json, component, bool);
}
