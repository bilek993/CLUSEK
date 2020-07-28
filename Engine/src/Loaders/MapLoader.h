#pragma once
#include <string>
#include <entt.hpp>
#include <json.hpp>
#include "Components/CameraComponentLoader.h"
#include "Components/ModelRenderComponentLoader.h"
#include "Components/TransformComponentLoader.h"
#include "Components/SkyboxComponentLoader.h"
#include "Components/PhysicsMaterialComponentLoader.h"
#include "Components/RigidbodyStaticPlaneComponentLoader.h"
#include "Components/RigidbodyStaticBoxComponentLoader.h"
#include "Components/RigidbodyDynamicBoxComponentLoader.h"
#include "Components/RigidbodyStaticSphereComponentLoader.h"
#include "Components/RigidbodyDynamicSphereComponentLoader.h"
#include "Components/RigidbodyStaticCapsuleComponentLoader.h"
#include "Components/RigidbodyDynamicCapsuleComponentLoader.h"
#include "Components/RigidbodyStaticCylinderComponentLoader.h"
#include "Components/RigidbodyDynamicCylinderComponentLoader.h"
#include "Components/WheelComponentLoader.h"
#include "Components/VehicleComponentLoader.h"
#include "Components/VehiclePlayerControllerComponentLoader.h"
#include "Components/TerrainComponentLoader.h"
#include "Components/RigidbodyStaticHeightFieldsComponentLoader.h"
#include "Components/CameraTargetComponentLoader.h"
#include "Components/CameraVehicleAnimatedTargetComponentLoader.h"
#include "Components/CommentComponentLoader.h"
#include "Components/GrassComponentLoader.h"

#define MAP_LOADER_TAGS(INPUT_TAG, EXPECTED_TAG, HASHED_TAG, REGISTRY, ENTITY) if (strcmp(INPUT_TAG, EXPECTED_TAG) == 0) REGISTRY.assign<entt::tag<HASHED_TAG>>(ENTITY);
#define MAP_COMPONENT_LOADERS(JSON, COMPONENT_ID, REGISTRY, ENTITY) if (JSON.key() == #COMPONENT_ID) COMPONENT_ID##LoaderInstance.Add(JSON.value(), REGISTRY, ENTITY);

class MapLoader final
{
public:
	static void CreateEntitiesFromMapFile(const std::string& path, entt::registry& registry);
private:
	static void ParseEntity(nlohmann::json& json, entt::registry& registry);
	static void AddTags(nlohmann::json& json, entt::registry& registry, const entt::registry::entity_type &entity);
	static void AddComponents(nlohmann::json& json, entt::registry& registry, const entt::registry::entity_type &entity);

	inline static CameraComponentLoader CameraComponentLoaderInstance;
	inline static CameraTargetComponentLoader CameraTargetComponentLoaderInstance;
	inline static CameraVehicleAnimatedTargetComponentLoader CameraVehicleAnimatedTargetComponentLoaderInstance;
	inline static TransformComponentLoader TransformComponentLoaderInstance;
	inline static ModelRenderComponentLoader ModelRenderComponentLoaderInstance;
	inline static SkyboxComponentLoader SkyboxComponentLoaderInstance;
	inline static PhysicsMaterialComponentLoader PhysicsMaterialComponentLoaderInstance;
	inline static RigidbodyStaticPlaneComponentLoader RigidbodyStaticPlaneComponentLoaderInstance;
	inline static RigidbodyStaticBoxComponentLoader RigidbodyStaticBoxComponentLoaderInstance;
	inline static RigidbodyDynamicBoxComponentLoader RigidbodyDynamicBoxComponentLoaderInstance;
	inline static RigidbodyStaticSphereComponentLoader RigidbodyStaticSphereComponentLoaderInstance;
	inline static RigidbodyDynamicSphereComponentLoader RigidbodyDynamicSphereComponentLoaderInstance;
	inline static RigidbodyStaticCapsuleComponentLoader RigidbodyStaticCapsuleComponentLoaderInstance;
	inline static RigidbodyDynamicCapsuleComponentLoader RigidbodyDynamicCapsuleComponentLoaderInstance;
	inline static RigidbodyStaticCylinderComponentLoader RigidbodyStaticCylinderComponentLoaderInstance;
	inline static RigidbodyDynamicCylinderComponentLoader RigidbodyDynamicCylinderComponentLoaderInstance;
	inline static RigidbodyStaticHeightFieldsComponentLoader RigidbodyStaticHeightFieldsComponentLoaderInstance;
	inline static VehicleComponentLoader VehicleComponentLoaderInstance;
	inline static WheelComponentLoader WheelComponentLoaderInstance;
	inline static VehiclePlayerControllerComponentLoader VehiclePlayerControllerComponentLoaderInstance;
	inline static TerrainComponentLoader TerrainComponentLoaderInstance;
	inline static CommentComponentLoader CommentComponentLoaderInstance;
	inline static GrassComponentLoader GrassComponentLoaderInstance;
};
