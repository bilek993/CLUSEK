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
	MAP_LOADER_BASIC_FIELD(MaxBrakeTorque, json, component, float);
	MAP_LOADER_BASIC_FIELD(MaxHandBrakeTorque, json, component, float);
	MAP_LOADER_BASIC_FIELD(MaxSteer, json, component, float);
	MAP_LOADER_BASIC_FIELD(AntiRollBarStiffness, json, component, float);
	MAP_LOADER_BASIC_FIELD(DifferentialType, json, component, std::string);
	MAP_LOADER_BASIC_FIELD(PeakTorque, json, component, float);
	MAP_LOADER_BASIC_FIELD(MaxOmega, json, component, float);
	MAP_LOADER_BASIC_FIELD(EngineMomentOfInertia, json, component, float);
	MAP_LOADER_BASIC_FIELD(GearsSwitchTime, json, component, float);
	MAP_LOADER_BASIC_FIELD(ClutchAccuracyMode, json, component, std::string);
	MAP_LOADER_BASIC_FIELD(ClutchStrength, json, component, float);
	MAP_LOADER_BASIC_FIELD(AckermannAccuracy, json, component, float);
}
