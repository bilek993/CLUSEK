#include "VehicleComponentLoader.h"
#include "../../ECS/Components/VehicleComponent.h"

void VehicleComponentLoader::Add(nlohmann::json& json, entt::registry& registry,
	const entt::registry::entity_type& entity)
{
	auto &component = registry.assign<VehicleComponent>(entity);

	MAP_LOADER_BASIC_FIELD(VehicleId, json, component, std::string);
	MAP_LOADER_BASIC_FIELD(Mass, json, component, float);
	MAP_LOADER_BASIC_FIELD(ConvexModelId, json, component, std::string);
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
	MAP_LOADER_BASIC_FIELD(GearboxType, json, component, std::string);
	MAP_LOADER_BASIC_FIELD(ClutchAccuracyMode, json, component, std::string);
	MAP_LOADER_BASIC_FIELD(ClutchStrength, json, component, float);
	MAP_LOADER_BASIC_FIELD(AckermannAccuracy, json, component, float);

	if (!json["ConvexPositionOffsetX"].is_null() && !json["ConvexPositionOffsetY"].is_null() && !json["ConvexPositionOffsetZ"].is_null())
	{
		const auto x = json["ConvexPositionOffsetX"].get<float>();
		const auto y = json["ConvexPositionOffsetY"].get<float>();
		const auto z = json["ConvexPositionOffsetZ"].get<float>();

		component.ConvexPositionOffsetVector = DirectX::XMFLOAT3(x, y, z);
	}

	if (!json["ConvexRotationOffsetX"].is_null() && !json["ConvexRotationOffsetY"].is_null() && !json["ConvexRotationOffsetZ"].is_null())
	{
		auto x = json["ConvexRotationOffsetX"].get<float>();
		auto y = json["ConvexRotationOffsetY"].get<float>();
		auto z = json["ConvexRotationOffsetZ"].get<float>();

		x *= M_PI / 180.0f;
		y *= M_PI / 180.0f;
		z *= M_PI / 180.0f;

		component.ConvexRotationOffsetVector = DirectX::XMFLOAT3(x, y, z);
	}
}
