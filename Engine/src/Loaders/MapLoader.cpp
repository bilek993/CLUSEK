#include "MapLoader.h"
#include "../Utils/Logger.h"
#include <fstream>
#include "../ECS/Components/TransformComponent.h"

void MapLoader::CreateEntitiesFromMapFile(const std::string& path, entt::registry& registry)
{
	Logger::Debug("Preparing to load map from path '" + path + "'...");

	nlohmann::json jsonObject;
	std::ifstream inputFile(path);
	inputFile >> jsonObject;

	for (auto it = jsonObject.begin(); it != jsonObject.end(); ++it)
	{
		ParseEntity(it.value(), registry);
	}
}

void MapLoader::ParseEntity(const nlohmann::json& json, entt::registry& registry)
{
	const auto entity = registry.create();
	AddTags(json["Tags"], registry, entity);
	AddComponents(json["Components"], registry, entity);
}

void MapLoader::AddTags(const nlohmann::json& json, entt::registry& registry, const entt::registry::entity_type &entity)
{
	for (auto it = json.begin(); it != json.end(); ++it)
	{
		const auto tag = it.value().get<std::string>().c_str();

		MAP_TAGS(tag, "Main Camera", registry, entity);
	}
}

void MapLoader::AddComponents(const nlohmann::json& json, entt::registry& registry, const entt::registry::entity_type &entity)
{
	for (auto it = json.begin(); it != json.end(); ++it)
	{
		if (it.key() == "CameraComponent")
			CameraComponentLoaderInstance.Add(it.value(), registry, entity);
		else if (it.key() == "TransformComponent")
			TransformComponentLoaderInstance.Add(it.value(), registry, entity);
		else if (it.key() == "RenderComponent")
			RenderComponentLoaderInstance.Add(it.value(), registry, entity);
	}
}
