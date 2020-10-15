#pragma once
#include <json.hpp>
#include "BaseWindow.h"
#include "../EntityCreatorMode.h"
#include "../../Loaders/Components/CameraComponentLoader.h"
#include "../../Loaders/Components/CameraTargetComponentLoader.h"
#include "../../Loaders/Components/CameraVehicleAnimatedTargetComponentLoader.h"
#include "../../Loaders/Components/CommentComponentLoader.h"
#include "../../Loaders/Components/GrassComponentLoader.h"
#include "../../Loaders/Components/ModelRenderComponentLoader.h"
#include "../../Loaders/Components/PhysicsMaterialComponentLoader.h"
#include "../../Loaders/Components/RigidbodyDynamicBoxComponentLoader.h"
#include "../../Loaders/Components/RigidbodyDynamicCapsuleComponentLoader.h"
#include "../../Loaders/Components/RigidbodyDynamicConvexComponentLoader.h"
#include "../../Loaders/Components/RigidbodyDynamicCylinderComponentLoader.h"
#include "../../Loaders/Components/RigidbodyDynamicSphereComponentLoader.h"
#include "../../Loaders/Components/RigidbodyStaticBoxComponentLoader.h"
#include "../../Loaders/Components/RigidbodyStaticCapsuleComponentLoader.h"
#include "../../Loaders/Components/RigidbodyStaticConvexComponentLoader.h"
#include "../../Loaders/Components/RigidbodyStaticCylinderComponentLoader.h"
#include "../../Loaders/Components/RigidbodyStaticHeightFieldsComponentLoader.h"
#include "../../Loaders/Components/RigidbodyStaticPlaneComponentLoader.h"
#include "../../Loaders/Components/RigidbodyStaticSphereComponentLoader.h"
#include "../../Loaders/Components/SkyboxComponentLoader.h"
#include "../../Loaders/Components/TerrainComponentLoader.h"
#include "../../Loaders/Components/TransformComponentLoader.h"
#include "../../Loaders/Components/VehicleComponentLoader.h"
#include "../../Loaders/Components/VehiclePlayerControllerComponentLoader.h"
#include "../../Loaders/Components/WheelComponentLoader.h"
#include "../../ECS/Systems/PhysicsSystem.h"

#define MAP_LOADER_TAGS(INPUT_TAG, EXPECTED_TAG, HASHED_TAG, REGISTRY, ENTITY) if (strcmp(INPUT_TAG, EXPECTED_TAG) == 0) REGISTRY.assign<entt::tag<HASHED_TAG>>(ENTITY);
#define MAP_COMPONENT_LOADERS(JSON, COMPONENT_ID, REGISTRY, ENTITY) if (JSON.key() == #COMPONENT_ID) COMPONENT_ID##LoaderInstance.Add(JSON.value(), REGISTRY, ENTITY);

class EntityCreatorWindow final : public BaseWindow
{
protected:
	void Draw() override;

private:
	void DrawCore();
	void DrawSettings();

	void HandleClicking();
	physx::PxRaycastBuffer RayCastPhysics(float maxDistance) const;
	
	void CreateEntityWithComponents();
	void CreateEntitiesWithComponents();

	void AddTags(nlohmann::json& json, const entt::registry::entity_type &entity) const;
	void AddComponents(nlohmann::json& json, const entt::registry::entity_type &entity) const;

	void RebuildEntities() const;

	EntityCreatorMode Mode = Single;
	bool ClearAfterCreating = true;
	bool UpdateTransformComponentOnClick = false;
	std::string EntityJsonText{};

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
	inline static RigidbodyStaticConvexComponentLoader RigidbodyStaticConvexComponentLoaderInstance;
	inline static RigidbodyDynamicConvexComponentLoader RigidbodyDynamicConvexComponentLoaderInstance;
	inline static VehicleComponentLoader VehicleComponentLoaderInstance;
	inline static WheelComponentLoader WheelComponentLoaderInstance;
	inline static VehiclePlayerControllerComponentLoader VehiclePlayerControllerComponentLoaderInstance;
	inline static TerrainComponentLoader TerrainComponentLoaderInstance;
	inline static CommentComponentLoader CommentComponentLoaderInstance;
	inline static GrassComponentLoader GrassComponentLoaderInstance;
};