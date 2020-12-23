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
	MAP_LOADER_BASIC_FIELD(TextureScaling, json, component, float);

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

	if (!json["MeshName"].is_null())
		component.Mesh.Name = json["MeshName"].get<std::string>();
}
