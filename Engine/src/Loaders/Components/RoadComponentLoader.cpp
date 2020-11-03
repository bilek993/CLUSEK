#include "RoadComponentLoader.h"

void RoadComponentLoader::Add(nlohmann::json& json, entt::registry& registry, const entt::registry::entity_type& entity)
{
	auto &component = registry.assign<RoadComponent>(entity);

	LoadPoints(component, json);
}

void RoadComponentLoader::LoadPoints(RoadComponent& component, nlohmann::json& json) const
{
	for (auto& pointJson : json["Points"])
	{
		DirectX::XMFLOAT3 point{};
		point.x = pointJson["PositionX"];
		point.y = pointJson["PositionY"];
		point.z = pointJson["PositionZ"];
		
		component.Points.emplace_back(point);
	}
}
