#include "RoadComponentLoader.h"

void RoadComponentLoader::Add(nlohmann::json& json, entt::registry& registry, const entt::registry::entity_type& entity)
{
	auto &component = registry.assign<RoadComponent>(entity);

	LoadPoints(component, json);
}

void RoadComponentLoader::LoadPoints(RoadComponent& component, nlohmann::json& json) const
{
	for (auto& pointJson : json["MeshVertices"])
	{
		DirectX::XMFLOAT2 point{};
		point.x = pointJson["PositionX"];
		point.y = pointJson["PositionY"];

		component.MeshVertices.emplace_back(point);
	}
	
	for (auto& pointJson : json["Points"])
	{
		DirectX::XMFLOAT3 point{};
		point.x = pointJson["PositionX"];
		point.y = pointJson["PositionY"];
		point.z = pointJson["PositionZ"];
		
		component.Points.emplace_back(point);
	}

	MAP_LOADER_BASIC_FIELD(Resolution, json, component, int);
	MAP_LOADER_BASIC_FIELD(SplitDistance, json, component, float);
	MAP_LOADER_BASIC_FIELD(MaterialId, json, component, std::string);
}
