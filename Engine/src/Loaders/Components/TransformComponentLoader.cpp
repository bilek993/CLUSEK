#include "TransformComponentLoader.h"
#include "../../ECS/Components/TransformComponent.h"

void TransformComponentLoader::Add(nlohmann::json& json, entt::registry& registry,
	const entt::registry::entity_type& entity)
{
	auto &component = registry.assign<TransformComponent>(entity);

	if (!json["RotationX"].is_null() && !json["RotationY"].is_null() && !json["RotationZ"].is_null())
	{
		auto x = json["RotationX"].get<float>();
		auto y = json["RotationY"].get<float>();
		auto z = json["RotationZ"].get<float>();

		component.InitialRotationVector = DirectX::XMFLOAT3(x, y, z);
	}

	if (!json["PositionX"].is_null() && !json["PositionY"].is_null() && !json["PositionZ"].is_null())
	{
		const auto x = json["PositionX"].get<float>();
		const auto y = json["PositionY"].get<float>();
		const auto z = json["PositionZ"].get<float>();

		component.InitialPositionVector = DirectX::XMFLOAT3(x, y, z);
	}
}
