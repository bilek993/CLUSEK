#include "VehiclePlayerControllerComponentLoader.h"
#include "../../ECS/Components/VehiclePlayerControllerComponent.h"

void VehiclePlayerControllerComponentLoader::Add(nlohmann::json& json, entt::registry& registry,
	const entt::registry::entity_type& entity)
{
	auto &component = registry.assign<VehiclePlayerControllerComponent>(entity);

	MAP_LOADER_BASIC_FIELD(Possessed, json, component, bool);
	MAP_LOADER_BASIC_FIELD(SteeringSpeed, json, component, float);
	MAP_LOADER_BASIC_FIELD(WheelReturningToNeutralPosition, json, component, float);
	MAP_LOADER_BASIC_FIELD(Reverse, json, component, bool);
}
