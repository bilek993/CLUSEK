#pragma once
#include <string>
#include <entt.hpp>
#include <json.hpp>
#include "Components/CameraComponentLoader.h"
#include "Components/RenderComponentLoader.h"
#include "Components/TransformComponentLoader.h"

#define MAP_LOADER_TAGS(INPUT_TAG, EXPECTED_TAG, REGISTRY, ENTITY) if (INPUT_TAG == EXPECTED_TAG) REGISTRY.assign<entt::tag<EXPECTED_TAG##_hs>>(ENTITY);
#define MAP_COMPONENT_LOADERS(JSON, COMPONENT_ID, REGISTRY, ENTITY) if (JSON.key() == #COMPONENT_ID) COMPONENT_ID##LoaderInstance.Add(JSON.value(), REGISTRY, ENTITY);

class MapLoader final
{
public:
	static void CreateEntitiesFromMapFile(const std::string& path, entt::registry& registry);
private:
	static void ParseEntity(const nlohmann::json& json, entt::registry& registry);
	static void AddTags(const nlohmann::json& json, entt::registry& registry, const entt::registry::entity_type &entity);
	static void AddComponents(const nlohmann::json& json, entt::registry& registry, const entt::registry::entity_type &entity);

	inline static CameraComponentLoader CameraComponentLoaderInstance;
	inline static TransformComponentLoader TransformComponentLoaderInstance;
	inline static RenderComponentLoader RenderComponentLoaderInstance;
};
