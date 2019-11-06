#include "WheelComponentLoader.h"
#include "../../ECS/Components/WheelComponent.h"

void WheelComponentLoader::Add(nlohmann::json& json, entt::registry& registry,
	const entt::registry::entity_type& entity)
{
	auto &component = registry.assign<WheelComponent>(entity);

	MAP_LOADER_BASIC_FIELD(VehicleId, json, component, std::string);
	MAP_LOADER_BASIC_FIELD(FlipWheel, json, component, bool);
	MAP_LOADER_BASIC_FIELD(WheelId, json, component, int);
	MAP_LOADER_BASIC_FIELD(Mass, json, component, float);
	MAP_LOADER_BASIC_FIELD(Width, json, component, float);
	MAP_LOADER_BASIC_FIELD(Radius, json, component, float);
	MAP_LOADER_BASIC_FIELD(TireType, json, component, int);
	MAP_LOADER_BASIC_FIELD(SuspensionMaxCompression, json, component, float);
	MAP_LOADER_BASIC_FIELD(SuspensionMaxDroop, json, component, float);
	MAP_LOADER_BASIC_FIELD(SuspensionSpringStrength, json, component, float);
	MAP_LOADER_BASIC_FIELD(SuspensionSpringDamperRate, json, component, float);
	MAP_LOADER_BASIC_FIELD(CamberAngleAtRest, json, component, float);
	MAP_LOADER_BASIC_FIELD(CamberAngleAtMaxDroop, json, component, float);
	MAP_LOADER_BASIC_FIELD(CamberAngleAtMaxCompression, json, component, float);
	MAP_LOADER_BASIC_FIELD(SuspensionForceApplicationPoinOffset, json, component, float);
	MAP_LOADER_BASIC_FIELD(TireForceApplicationPoinOffset, json, component, float);
	MAP_LOADER_BASIC_FIELD(LatStiffXMultiplier, json, component, float);
	MAP_LOADER_BASIC_FIELD(LatStiffYMultiplier, json, component, float);
}
