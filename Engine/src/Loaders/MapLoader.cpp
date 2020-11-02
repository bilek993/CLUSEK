#include "MapLoader.h"
#include "../Utils/Logger.h"
#include <fstream>
#include "../Tags.h"
#include "../Utils/StringUtil.h"

void MapLoader::CreateEntitiesFromMapFile(const std::string& path, entt::registry& registry)
{
	Logger::Debug("Preparing to load map from path '" + path + "'...");

	nlohmann::json jsonObjectExportMap;
	std::ifstream inputFileExportMap(path);
	inputFileExportMap >> jsonObjectExportMap;

	for (auto itE = jsonObjectExportMap.begin(); itE != jsonObjectExportMap.end(); ++itE)
	{
		const auto pathToImportedMap = StringUtil::FindDirectory(path) + "/" + itE->get<std::string>();
		Logger::Debug("Importing file from: " + pathToImportedMap);
		
		nlohmann::json jsonObjectImportedMap;
		std::ifstream inputFileImportedMap(pathToImportedMap);
		inputFileImportedMap >> jsonObjectImportedMap;

		for (auto itI = jsonObjectImportedMap.begin(); itI != jsonObjectImportedMap.end(); ++itI)
		{
			ParseEntity(itI.value(), registry);
		}
	}
}

void MapLoader::ParseEntity(nlohmann::json& json, entt::registry& registry)
{
	const auto entity = registry.create();
	AddTags(json["Tags"], registry, entity);
	AddComponents(json["Components"], registry, entity);
}

void MapLoader::AddTags(nlohmann::json& json, entt::registry& registry, const entt::registry::entity_type &entity)
{
	const auto tags = json.get<std::vector<std::string>>();
	registry.assign<entt::tag<Tags::REQUIRES_REBUILD>>(entity);

	for (auto& tag : tags)
	{
		MAP_LOADER_TAGS(tag.c_str(), "Main Camera", Tags::MAIN_CAMERA, registry, entity);
	}
}

void MapLoader::AddComponents(nlohmann::json& json, entt::registry& registry, const entt::registry::entity_type &entity)
{
	for (auto it = json.begin(); it != json.end(); ++it)
	{
		MAP_COMPONENT_LOADERS(it, CameraComponent, registry, entity);
		MAP_COMPONENT_LOADERS(it, CameraTargetComponent, registry, entity);
		MAP_COMPONENT_LOADERS(it, CameraVehicleAnimatedTargetComponent, registry, entity);
		MAP_COMPONENT_LOADERS(it, TransformComponent, registry, entity);
		MAP_COMPONENT_LOADERS(it, ModelRenderComponent, registry, entity);
		MAP_COMPONENT_LOADERS(it, SkyboxComponent, registry, entity);
		MAP_COMPONENT_LOADERS(it, PhysicsMaterialComponent, registry, entity);
		MAP_COMPONENT_LOADERS(it, RigidbodyStaticPlaneComponent, registry, entity);
		MAP_COMPONENT_LOADERS(it, RigidbodyStaticBoxComponent, registry, entity);
		MAP_COMPONENT_LOADERS(it, RigidbodyDynamicBoxComponent, registry, entity);
		MAP_COMPONENT_LOADERS(it, RigidbodyStaticSphereComponent, registry, entity);
		MAP_COMPONENT_LOADERS(it, RigidbodyDynamicSphereComponent, registry, entity);
		MAP_COMPONENT_LOADERS(it, RigidbodyStaticCapsuleComponent, registry, entity);
		MAP_COMPONENT_LOADERS(it, RigidbodyDynamicCapsuleComponent, registry, entity);
		MAP_COMPONENT_LOADERS(it, RigidbodyStaticCylinderComponent, registry, entity);
		MAP_COMPONENT_LOADERS(it, RigidbodyDynamicCylinderComponent, registry, entity);
		MAP_COMPONENT_LOADERS(it, RigidbodyStaticHeightFieldsComponent, registry, entity);
		MAP_COMPONENT_LOADERS(it, RigidbodyStaticConvexComponent, registry, entity);
		MAP_COMPONENT_LOADERS(it, RigidbodyDynamicConvexComponent, registry, entity);
		MAP_COMPONENT_LOADERS(it, VehicleComponent, registry, entity);
		MAP_COMPONENT_LOADERS(it, WheelComponent, registry, entity);
		MAP_COMPONENT_LOADERS(it, VehiclePlayerControllerComponent, registry, entity);
		MAP_COMPONENT_LOADERS(it, TerrainComponent, registry, entity);
		MAP_COMPONENT_LOADERS(it, CommentComponent, registry, entity);
		MAP_COMPONENT_LOADERS(it, GrassComponent, registry, entity);
		MAP_COMPONENT_LOADERS(it, RoadComponent, registry, entity);
	}
}
