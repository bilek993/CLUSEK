#include "WheelComponentLoader.h"
#include "../../ECS/Components/WheelComponent.h"

void WheelComponentLoader::Add(nlohmann::json& json, entt::registry& registry,
	const entt::registry::entity_type& entity)
{
	auto &component = registry.assign<WheelComponent>(entity);

	MAP_LOADER_BASIC_FIELD(VehicleId, json, component, std::string);
	MAP_LOADER_BASIC_FIELD(WheelId, json, component, int);
	MAP_LOADER_BASIC_FIELD(Mass, json, component, float);
	MAP_LOADER_BASIC_FIELD(Width, json, component, float);
	MAP_LOADER_BASIC_FIELD(Radius, json, component, float);
}
