#include "CameraTargetComponentLoader.h"
#include "../../ECS/Components/CameraTargetComponent.h"

void CameraTargetComponentLoader::Add(nlohmann::json& json, entt::registry& registry,
	const entt::registry::entity_type& entity)
{
	auto &component = registry.assign<CameraTargetComponent>(entity);

	MAP_LOADER_BASIC_FIELD(Active, json, component, bool);
	MAP_LOADER_BASIC_FIELD(RotationLag, json, component, float);

	if (!json["PaddingRotationX"].is_null() && !json["PaddingRotationY"].is_null() && !json["PaddingRotationZ"].is_null())
	{
		auto x = json["PaddingRotationX"].get<float>();
		auto y = json["PaddingRotationY"].get<float>();
		auto z = json["PaddingRotationZ"].get<float>();

		x *= M_PI / 180.0f;
		y *= M_PI / 180.0f;
		z *= M_PI / 180.0f;

		component.InitialRotationVector = DirectX::XMFLOAT3(x, y, z);
	}

	if (!json["PaddingPositionX"].is_null() && !json["PaddingPositionY"].is_null() && !json["PaddingPositionZ"].is_null())
	{
		const auto x = json["PaddingPositionX"].get<float>();
		const auto y = json["PaddingPositionY"].get<float>();
		const auto z = json["PaddingPositionZ"].get<float>();

		component.InitialPositionVector = DirectX::XMFLOAT3(x, y, z);
	}
}
