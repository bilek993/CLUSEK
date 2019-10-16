#include "VehicleComponentLoader.h"
#include "../../ECS/Components/VehicleComponent.h"

void VehicleComponentLoader::Add(nlohmann::json& json, entt::registry& registry,
	const entt::registry::entity_type& entity)
{
	auto &component = registry.assign<VehicleComponent>(entity);

	MAP_LOADER_BASIC_FIELD(VehicleId, json, component, std::string);
	MAP_LOADER_BASIC_FIELD(Mass, json, component, float);
	MAP_LOADER_BASIC_FIELD(DimensionX, json, component, float);
	MAP_LOADER_BASIC_FIELD(DimensionY, json, component, float);
	MAP_LOADER_BASIC_FIELD(DimensionZ, json, component, float);
	MAP_LOADER_BASIC_FIELD(CenterOfMassOffsetX, json, component, float);
	MAP_LOADER_BASIC_FIELD(CenterOfMassOffsetY, json, component, float);
	MAP_LOADER_BASIC_FIELD(CenterOfMassOffsetZ, json, component, float);
}
