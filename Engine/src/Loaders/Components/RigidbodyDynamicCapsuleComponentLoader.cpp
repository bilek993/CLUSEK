#include "RigidbodyDynamicCapsuleComponentLoader.h"
#include "../../ECS/Components/RigidbodyDynamicCapsuleComponent.h"

void RigidbodyDynamicCapsuleComponentLoader::Add(nlohmann::json& json, entt::registry& registry,
	const entt::registry::entity_type& entity)
{
	auto &component = registry.assign<RigidbodyDynamicCapsuleComponent>(entity);

	MAP_LOADER_BASIC_FIELD(Height, json, component, float);
	MAP_LOADER_BASIC_FIELD(Radius, json, component, float);
	MAP_LOADER_BASIC_FIELD(Density, json, component, float);

	if (!json["PositionOffsetX"].is_null() && !json["PositionOffsetY"].is_null() && !json["PositionOffsetZ"].is_null())
	{
		const auto x = json["PositionOffsetX"].get<float>();
		const auto y = json["PositionOffsetY"].get<float>();
		const auto z = json["PositionOffsetZ"].get<float>();

		component.PositionOffsetVector = DirectX::XMFLOAT3(x, y, z);
	}

	if (!json["RotationOffsetX"].is_null() && !json["RotationOffsetY"].is_null() && !json["RotationOffsetZ"].is_null())
	{
		auto x = json["RotationOffsetX"].get<float>();
		auto y = json["RotationOffsetY"].get<float>();
		auto z = json["RotationOffsetZ"].get<float>();

		x *= M_PI / 180.0f;
		y *= M_PI / 180.0f;
		z *= M_PI / 180.0f;

		component.RotationOffsetVector = DirectX::XMFLOAT3(x, y, z);
	}
}
