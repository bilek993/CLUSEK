#include "CameraVehicleAnimatedTargetComponentLoader.h"
#include "../../ECS/Components/CameraVehicleAnimatedTargetComponent.h"

void CameraVehicleAnimatedTargetComponentLoader::Add(nlohmann::json& json, entt::registry& registry,
	const entt::registry::entity_type& entity)
{
	auto &component = registry.assign<CameraVehicleAnimatedTargetComponent>(entity);
}
