#include "CameraVehicleAnimatedTargetComponentLoader.h"
#include "../../ECS/Components/CameraVehicleAnimatedTargetComponent.h"

void CameraVehicleAnimatedTargetComponentLoader::Add(nlohmann::json& json, entt::registry& registry,
	const entt::registry::entity_type& entity)
{
	auto &component = registry.assign<CameraVehicleAnimatedTargetComponent>(entity);

	MAP_LOADER_BASIC_FIELD(Power, json, component, float);
	MAP_LOADER_BASIC_FIELD(Smoothness, json, component, float);

	if (!json["AnimationPositionX"].is_null() && !json["AnimationPositionY"].is_null() && !json["AnimationPositionZ"].is_null())
	{
		const auto x = json["AnimationPositionX"].get<float>();
		const auto y = json["AnimationPositionY"].get<float>();
		const auto z = json["AnimationPositionZ"].get<float>();

		const auto positionFloats = DirectX::XMFLOAT3(x, y, z);
		component.AnimationPositionVector = XMLoadFloat3(&positionFloats);
		component.CurrentAnimationPositionVector = XMLoadFloat3(&positionFloats);
	}
}
