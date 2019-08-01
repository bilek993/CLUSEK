#include "TransformComponentLoader.h"
#include "../../ECS/Components/TransformComponent.h"
#include "../../Renderer/TransformLogic.h"

void TransformComponentLoader::Add(nlohmann::json& json, entt::registry& registry,
	const entt::registry::entity_type& entity)
{
	auto &component = registry.assign<TransformComponent>(entity);

	if (!json["RotationX"].is_null() && !json["RotationY"].is_null() && !json["RotationZ"].is_null())
	{
		const auto x = json["RotationX"].get<float>();
		const auto y = json["RotationY"].get<float>();
		const auto z = json["RotationZ"].get<float>();
		TransformLogic::SetRotation(x, y, z, component);
	}

	if (!json["PositionX"].is_null() && !json["PositionY"].is_null() && !json["PositionZ"].is_null())
	{
		const auto x = json["PositionX"].get<float>();
		const auto y = json["PositionY"].get<float>();
		const auto z = json["PositionZ"].get<float>();
		TransformLogic::SetPosition(x, y, z, component);
	}
}
