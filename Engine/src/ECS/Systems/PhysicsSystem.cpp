#include "PhysicsSystem.h"
#include <thread>
#include "../Components/PhysicsMaterialComponent.h"
#include "../Components/RigidbodyStaticPlaneComponent.h"
#include "../Components/RigidbodyStaticBoxComponent.h"
#include "../Components/TransformComponent.h"
#include "../../Renderer/TransformLogic.h"
#include "../Components/RigidbodyDynamicBoxComponent.h"
#include "../Components/RigidbodyStaticSphereComponent.h"
#include "../Components/RigidbodyDynamicSphereComponent.h"
#include "../../Tags.h"
#include "../../Loaders/ModelLoader.h"
#include "../Components/RigidbodyStaticCapsuleComponent.h"
#include "../Components/RigidbodyDynamicCapsuleComponent.h"
#include "../../Physics/PhysicsMeshGenerator.h"
#include "../Components/RigidbodyDynamicCylinderComponent.h"
#include "../Components/WheelComponent.h"
#include "../Components/VehicleComponent.h"
#include "../../Physics/VehicleResourcesGenerator.h"
#include "../../Physics/PhysicsFilterHelper.h"
#include "../Components/RigidbodyStaticHeightFieldsComponent.h"
#include "../Components/TerrainComponent.h"
#include "../../Utils/TerrainUtil.h"
#include "../../Physics/PhysicsFilterShader.h"
#include "../../Loaders/PhysicsMaterialLoader.h"
#include "../../Loaders/ResourcesLoader.h"
#include "../Components/RigidbodyDynamicConvexComponent.h"
#include "../Components/RigidbodyStaticConvexComponent.h"
#include "../Components/RigidbodyStaticCylinderComponent.h"
#include "../Components/RoadComponent.h"

void PhysicsSystem::Start()
{
	Logger::Debug("Staring physics system...");

	InitializeCore();
	InitializeResources();
	InitializePhysicsMaterialManager();
}

void PhysicsSystem::Rebuild()
{
	Logger::Debug("Rebuilding physics system...");

	InitializePhysicsMaterialComponents();
	
	InitializeRigidbodyStaticPlaneComponents();
	InitializeRigidbodyStaticBoxComponents();
	InitializeRigidbodyDynamicBoxComponents();
	InitializeRigidbodyStaticSphereComponents();
	InitializeRigidbodyDynamicSphereComponents();
	InitializeRigidbodyStaticCapsuleComponents();
	InitializeRigidbodyDynamicCapsuleComponents();
	InitializeRigidbodyStaticCylinderComponents();
	InitializeRigidbodyDynamicCylinderComponents();
	InitializeRigidbodyStaticHeightFields();
	InitializeRigidbodyStaticConvexComponent();
	InitializeRigidbodyDynamicConvexComponent();
	
	InitializeRoadComponent();
	
	InitializeVehiclesAndWheels();
}

void PhysicsSystem::Update(const float deltaTime)
{
	TimeSum += deltaTime;

	while (TimeSum >= ConfigurationData->PhysicsDeltaTime)
	{
		TimeSum -= ConfigurationData->PhysicsDeltaTime;
		UpdateSimulation();
		UpdateVehicles();
		UpdateMatrices();
	}
}

physx::PxSimulationStatistics PhysicsSystem::GetStatistics() const
{
	physx::PxSimulationStatistics statistics;
	Scene->getSimulationStatistics(statistics);
	return statistics;
}

physx::PxPhysics* PhysicsSystem::GetPointerToInternalPhysics() const
{
	return Physics;
}

std::shared_ptr<PhysicsMaterialManager> PhysicsSystem::GetPhysicsMaterialManagerSmartPointer() const
{
	return MaterialManager;
}

physx::PxScene* PhysicsSystem::GetPointerToScene() const
{
	return Scene;
}

void PhysicsSystem::UpdateFrictionPairs()
{
	Logger::Debug("Updating friction pairs in progress...");

	PX_RELEASE(FrictionPairs);
	FrictionPairs = VehicleResourcesGenerator::CreateFrictionPairs(*Physics, MaterialManager.get());

	Logger::Debug("Friction pairs have been updated!");
}

PhysicsSystem::~PhysicsSystem()
{
	MaterialManager->ReleaseAllMaterials();

	const auto numberOfActors = Scene->getNbActors(physx::PxActorTypeFlag::eRIGID_DYNAMIC | physx::PxActorTypeFlag::eRIGID_STATIC);

	if (numberOfActors > 0)
	{
		std::vector<physx::PxRigidActor*> actors(numberOfActors);
		Scene->getActors(physx::PxActorTypeFlag::eRIGID_DYNAMIC | physx::PxActorTypeFlag::eRIGID_STATIC,
			reinterpret_cast<physx::PxActor**>(&actors[0]), numberOfActors);

		for (auto actor : actors)
			PX_RELEASE(actor);
	}

	Registry->view<RigidbodyStaticHeightFieldsComponent>().each(
		[this](RigidbodyStaticHeightFieldsComponent &rigidbodyStaticHeightFieldsComponent)
	{
		Allocator.deallocate(rigidbodyStaticHeightFieldsComponent.HeightFieldSample);
		PX_RELEASE(rigidbodyStaticHeightFieldsComponent.HeightField);
	});

	Registry->view<VehicleComponent>().each([](VehicleComponent &vehicleComponent)
	{
		PX_RELEASE(vehicleComponent.Vehicle);
	});

	for (auto& convexMesh : ModelLoader::GetAllConvexResources())
		PX_RELEASE(convexMesh);

	VehicleSceneQueryData->Free(Allocator);
	PX_RELEASE(BatchQuery);
	PX_RELEASE(FrictionPairs);

	physx::PxCloseVehicleSDK();

	PX_RELEASE(Scene);
	PX_RELEASE(Dispatcher);
	PX_RELEASE(Physics);

	if (Pvd)
	{
		auto transport = Pvd->getTransport();

		PX_RELEASE(Pvd);
		PX_RELEASE(transport);
	}

	PX_RELEASE(Cooking);
	PX_RELEASE(Foundation);
}

void PhysicsSystem::InitializeCore()
{
	const auto version = PX_PHYSICS_VERSION;
	Logger::Debug("Physics Library used: PhysX (" + std::to_string(version) + ") by Nvidia.");

	Foundation = PxCreateFoundation(version, Allocator, ErrorCallback);
	if (!Foundation)
		Logger::Error("Error creating physics foundation");

#ifdef _DEBUG
	Logger::Warning("PhysX is in debug mode and PhysX Visual Debugger is enabled! Physics will be much slower!");

	Pvd = PxCreatePvd(*Foundation);

	const auto transport = physx::PxDefaultPvdSocketTransportCreate(ConfigurationData->PvdSocketHost.c_str(), ConfigurationData->PvdSocketPort, ConfigurationData->PvdSocketTimeout);
	Pvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);
#endif

	Physics = PxCreatePhysics(PX_PHYSICS_VERSION, *Foundation, physx::PxTolerancesScale(), true, Pvd);

	const auto numberOfThreads = std::thread::hardware_concurrency();

	physx::PxSceneDesc sceneDesc(Physics->getTolerancesScale());
	sceneDesc.gravity = physx::PxVec3(ConfigurationData->GravityX, ConfigurationData->GravityY, ConfigurationData->GravityZ);
	Dispatcher = physx::PxDefaultCpuDispatcherCreate(numberOfThreads);
	sceneDesc.cpuDispatcher = Dispatcher;
	sceneDesc.filterShader = PhysicsFilterShader::Filter;

	PxRegisterHeightFields(*Physics);

	Scene = Physics->createScene(sceneDesc);

	Cooking = PxCreateCooking(PX_PHYSICS_VERSION, *Foundation, physx::PxCookingParams(physx::PxTolerancesScale()));

#ifdef _DEBUG
	auto pvdClient = Scene->getScenePvdClient();;
	pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
	pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
	pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
#endif

	PxInitVehicleSDK(*Physics);
	PxVehicleSetBasisVectors(physx::PxVec3(0, 1, 0), physx::PxVec3(0, 0, 1));
	PxVehicleSetUpdateMode(ConfigurationData->VehicleUpdateMode == "ACCELERATION" ? physx::PxVehicleUpdateMode::eACCELERATION : physx::PxVehicleUpdateMode::eVELOCITY_CHANGE);
	physx::PxVehicleSetSweepHitRejectionAngles(ConfigurationData->VehicleSweepPointRejectionAngle, ConfigurationData->VehicleSweepNormalRejectionAngle);
	physx::PxVehicleSetMaxHitActorAcceleration(ConfigurationData->VehicleMaxHitAcceleration);

	const auto vehiclesCount = Registry->view<VehicleComponent>().size();

	VehicleSceneQueryData = VehicleSceneQueryData::Allocate(vehiclesCount,
															4, 
															ConfigurationData->VehicleMaxHitPointsPerWheel,
															vehiclesCount,
															Allocator);

	BatchQuery = VehicleSceneQueryData::SetUpBatchedSceneQuery(0, *VehicleSceneQueryData, Scene);
}

void PhysicsSystem::InitializeResources() const
{
	ResourcesLoader::LoadFromPhysics(*Physics, *Cooking, ConfigurationData);
}

void PhysicsSystem::InitializePhysicsMaterialManager()
{
	Logger::Debug("Preparing to initialize physics material manager...");
	
	MaterialManager = std::make_unique<PhysicsMaterialManager>(	Physics,
																ConfigurationData->DefaultPhysicsMaterialStaticFriction,
																ConfigurationData->DefaultPhysicsMaterialDynamicFriction,
																ConfigurationData->DefaultPhysicsMaterialRestitution);

	PhysicsMaterialLoader::Load(ConfigurationData->PathToPhysicsMaterials, MaterialManager.get(), Physics);
}

void PhysicsSystem::InitializePhysicsMaterialComponents()
{
	Logger::Debug("Preparing to initialize physics materials...");

	Registry->view<PhysicsMaterialComponent, entt::tag<Tags::REQUIRES_REBUILD>>().each([this](PhysicsMaterialComponent &physicsMaterialComponent, auto _)
	{
		physicsMaterialComponent.Material = MaterialManager->GetMaterialByName(physicsMaterialComponent.Name);
		Logger::Debug("Paired material '" + physicsMaterialComponent.Name + "' with proper component.");
	});

	UpdateFrictionPairs();
}

void PhysicsSystem::InitializeRigidbodyStaticPlaneComponents()
{
	Logger::Debug("Preparing to initialize rigidbody static planes...");

	Registry->view<PhysicsMaterialComponent, RigidbodyStaticPlaneComponent, entt::tag<Tags::REQUIRES_REBUILD>>().each(
		[this](PhysicsMaterialComponent &physicsMaterialComponent, RigidbodyStaticPlaneComponent &rigidbodyStaticPlaneComponent, auto _)
	{
		rigidbodyStaticPlaneComponent.Body = PxCreatePlane(*Physics, physx::PxPlane(rigidbodyStaticPlaneComponent.NormalX, 
																					rigidbodyStaticPlaneComponent.NormalY, 
																					rigidbodyStaticPlaneComponent.NormalZ, 
																					rigidbodyStaticPlaneComponent.Distance), *physicsMaterialComponent.Material);

		SetFiltersForComponent(	*rigidbodyStaticPlaneComponent.Body,
								physicsMaterialComponent.SurfaceFilterType);

		Scene->addActor(*rigidbodyStaticPlaneComponent.Body);
	});
}

void PhysicsSystem::InitializeRigidbodyStaticBoxComponents()
{
	Logger::Debug("Preparing to initialize rigidbody static boxes...");

	Registry->view<TransformComponent, PhysicsMaterialComponent, RigidbodyStaticBoxComponent, entt::tag<Tags::REQUIRES_REBUILD>>().each(
		[this](TransformComponent &transformComponent,PhysicsMaterialComponent &physicsMaterialComponent, RigidbodyStaticBoxComponent &rigidbodyStaticBoxComponent, auto _)
	{
		const auto geometry = physx::PxBoxGeometry(rigidbodyStaticBoxComponent.Width / 2, rigidbodyStaticBoxComponent.Height / 2, rigidbodyStaticBoxComponent.Depth / 2);
		const auto transform = CalculatePxTransform(transformComponent);

		rigidbodyStaticBoxComponent.Body = PxCreateStatic(	*Physics, 
															transform,
															geometry,
															*physicsMaterialComponent.Material,
															CalculateOffsetPxTransform(rigidbodyStaticBoxComponent));

		SetFiltersForComponent(	*rigidbodyStaticBoxComponent.Body, 
								physicsMaterialComponent.SurfaceFilterType);

		Scene->addActor(*rigidbodyStaticBoxComponent.Body);
	});
}

void PhysicsSystem::InitializeRigidbodyDynamicBoxComponents()
{
	Logger::Debug("Preparing to initialize rigidbody dynamic boxes...");

	Registry->view<TransformComponent, PhysicsMaterialComponent, RigidbodyDynamicBoxComponent, entt::tag<Tags::REQUIRES_REBUILD>>().each(
		[this](TransformComponent &transformComponent, PhysicsMaterialComponent &physicsMaterialComponent, RigidbodyDynamicBoxComponent &rigidbodyDynamicBoxComponent, auto _)
	{
		const auto geometry = physx::PxBoxGeometry(rigidbodyDynamicBoxComponent.Width / 2, rigidbodyDynamicBoxComponent.Height / 2, rigidbodyDynamicBoxComponent.Depth / 2);
		const auto transform = CalculatePxTransform(transformComponent);

		rigidbodyDynamicBoxComponent.Body = PxCreateDynamic(*Physics,
															transform,
															geometry,
															*physicsMaterialComponent.Material,
															rigidbodyDynamicBoxComponent.Density,
															CalculateOffsetPxTransform(rigidbodyDynamicBoxComponent));

		SetFiltersForComponent(	*rigidbodyDynamicBoxComponent.Body,
								physicsMaterialComponent.SurfaceFilterType);

		Scene->addActor(*rigidbodyDynamicBoxComponent.Body);
	});
}

void PhysicsSystem::InitializeRigidbodyStaticSphereComponents()
{
	Logger::Debug("Preparing to initialize rigidbody static spheres...");

	Registry->view<TransformComponent, PhysicsMaterialComponent, RigidbodyStaticSphereComponent, entt::tag<Tags::REQUIRES_REBUILD>>().each(
		[this](TransformComponent &transformComponent, PhysicsMaterialComponent &physicsMaterialComponent, RigidbodyStaticSphereComponent &rigidbodyStaticSphereComponent, auto _)
	{
		const auto geometry = physx::PxSphereGeometry(rigidbodyStaticSphereComponent.Radius);
		const auto transform = CalculatePxTransform(transformComponent);

		rigidbodyStaticSphereComponent.Body = PxCreateStatic(	*Physics,
																transform,
																geometry,
																*physicsMaterialComponent.Material,
																CalculateOffsetPxTransform(rigidbodyStaticSphereComponent));

		SetFiltersForComponent(	*rigidbodyStaticSphereComponent.Body, 
								physicsMaterialComponent.SurfaceFilterType);

		Scene->addActor(*rigidbodyStaticSphereComponent.Body);
	});
}

void PhysicsSystem::InitializeRigidbodyDynamicSphereComponents()
{
	Logger::Debug("Preparing to initialize rigidbody dynamic spheres...");

	Registry->view<TransformComponent, PhysicsMaterialComponent, RigidbodyDynamicSphereComponent, entt::tag<Tags::REQUIRES_REBUILD>>().each(
		[this](TransformComponent &transformComponent, PhysicsMaterialComponent &physicsMaterialComponent, RigidbodyDynamicSphereComponent &rigidbodyDynamicSphereComponent, auto _)
	{
		const auto geometry = physx::PxSphereGeometry(rigidbodyDynamicSphereComponent.Radius);
		const auto transform = CalculatePxTransform(transformComponent);

		rigidbodyDynamicSphereComponent.Body = PxCreateDynamic(	*Physics,
																transform,
																geometry,
																*physicsMaterialComponent.Material,
																rigidbodyDynamicSphereComponent.Density,
																CalculateOffsetPxTransform(rigidbodyDynamicSphereComponent));

		SetFiltersForComponent(	*rigidbodyDynamicSphereComponent.Body,
								physicsMaterialComponent.SurfaceFilterType);

		Scene->addActor(*rigidbodyDynamicSphereComponent.Body);
	});
}

void PhysicsSystem::InitializeRigidbodyStaticCapsuleComponents()
{
	Logger::Debug("Preparing to initialize rigidbody static capsules...");

	Registry->view<TransformComponent, PhysicsMaterialComponent, RigidbodyStaticCapsuleComponent, entt::tag<Tags::REQUIRES_REBUILD>>().each(
		[this](TransformComponent &transformComponent, PhysicsMaterialComponent &physicsMaterialComponent, RigidbodyStaticCapsuleComponent &rigidbodyStaticCapsuleComponent, auto _)
	{
		const auto geometry = physx::PxCapsuleGeometry(rigidbodyStaticCapsuleComponent.Radius, rigidbodyStaticCapsuleComponent.Height / 2);
		const auto transform = CalculatePxTransform(transformComponent);

		rigidbodyStaticCapsuleComponent.Body = PxCreateStatic(	*Physics,
																transform,
																geometry,
																*physicsMaterialComponent.Material,
																CalculateOffsetPxTransform(rigidbodyStaticCapsuleComponent));

		SetFiltersForComponent(	*rigidbodyStaticCapsuleComponent.Body, 
								physicsMaterialComponent.SurfaceFilterType);

		Scene->addActor(*rigidbodyStaticCapsuleComponent.Body);
	});
}

void PhysicsSystem::InitializeRigidbodyDynamicCapsuleComponents()
{
	Logger::Debug("Preparing to initialize rigidbody dynamic capsules...");

	Registry->view<TransformComponent, PhysicsMaterialComponent, RigidbodyDynamicCapsuleComponent, entt::tag<Tags::REQUIRES_REBUILD>>().each(
		[this](TransformComponent &transformComponent, PhysicsMaterialComponent &physicsMaterialComponent, RigidbodyDynamicCapsuleComponent &rigidbodyDynamicCapsuleComponent, auto _)
	{
		const auto geometry = physx::PxCapsuleGeometry(rigidbodyDynamicCapsuleComponent.Radius, rigidbodyDynamicCapsuleComponent.Height / 2);
		const auto transform = CalculatePxTransform(transformComponent);

		rigidbodyDynamicCapsuleComponent.Body = PxCreateDynamic(*Physics,
																transform,
																geometry,
																*physicsMaterialComponent.Material,
																rigidbodyDynamicCapsuleComponent.Density,
																CalculateOffsetPxTransform(rigidbodyDynamicCapsuleComponent));

		SetFiltersForComponent(	*rigidbodyDynamicCapsuleComponent.Body, 
								physicsMaterialComponent.SurfaceFilterType);

		Scene->addActor(*rigidbodyDynamicCapsuleComponent.Body);
	});
}

void PhysicsSystem::InitializeRigidbodyStaticCylinderComponents()
{
	Logger::Debug("Preparing to initialize rigidbody static cylinders...");

	Registry->view<TransformComponent, PhysicsMaterialComponent, RigidbodyStaticCylinderComponent, entt::tag<Tags::REQUIRES_REBUILD>>().each(
		[this](TransformComponent &transformComponent, PhysicsMaterialComponent &physicsMaterialComponent, RigidbodyStaticCylinderComponent &rigidbodyStaticCylinderComponent, auto _)
	{
		const auto geometry = physx::PxConvexMeshGeometry(
			PhysicsMeshGenerator::CreateCylinder(*Physics, *Cooking, rigidbodyStaticCylinderComponent.Width, rigidbodyStaticCylinderComponent.Radius));
		const auto transform = CalculatePxTransform(transformComponent);

		rigidbodyStaticCylinderComponent.Body = PxCreateStatic(	*Physics,
																transform,
																geometry,
																*physicsMaterialComponent.Material,
																CalculateOffsetPxTransform(rigidbodyStaticCylinderComponent));

		SetFiltersForComponent(	*rigidbodyStaticCylinderComponent.Body,
								physicsMaterialComponent.SurfaceFilterType);

		Scene->addActor(*rigidbodyStaticCylinderComponent.Body);
	});
}

void PhysicsSystem::InitializeRigidbodyDynamicCylinderComponents()
{
	Logger::Debug("Preparing to initialize rigidbody dynamic cylinders...");

	Registry->view<TransformComponent, PhysicsMaterialComponent, RigidbodyDynamicCylinderComponent, entt::tag<Tags::REQUIRES_REBUILD>>().each(
		[this](TransformComponent &transformComponent, PhysicsMaterialComponent &physicsMaterialComponent, RigidbodyDynamicCylinderComponent &rigidbodyDynamicCylinderComponent, auto _)
	{
		const auto geometry = physx::PxConvexMeshGeometry(
			PhysicsMeshGenerator::CreateCylinder(*Physics, *Cooking, rigidbodyDynamicCylinderComponent.Width, rigidbodyDynamicCylinderComponent.Radius));
		const auto transform = CalculatePxTransform(transformComponent);

		rigidbodyDynamicCylinderComponent.Body = PxCreateDynamic(*Physics,
																transform,
																geometry,
																*physicsMaterialComponent.Material,
																rigidbodyDynamicCylinderComponent.Density,
																CalculateOffsetPxTransform(rigidbodyDynamicCylinderComponent));

		SetFiltersForComponent(	*rigidbodyDynamicCylinderComponent.Body, 
								physicsMaterialComponent.SurfaceFilterType);

		Scene->addActor(*rigidbodyDynamicCylinderComponent.Body);
	});
}

void PhysicsSystem::InitializeRigidbodyStaticConvexComponent()
{
	Logger::Debug("Preparing to initialize rigidbody static convex...");

	Registry->view<TransformComponent, PhysicsMaterialComponent, RigidbodyStaticConvexComponent, entt::tag<Tags::REQUIRES_REBUILD>>().each(
		[this](TransformComponent &transformComponent, PhysicsMaterialComponent &physicsMaterialComponent, RigidbodyStaticConvexComponent &rigidbodyStaticConvexComponent, auto _)
	{
		const auto geometry = physx::PxConvexMeshGeometry(ModelLoader::GetConvexResource(rigidbodyStaticConvexComponent.ConvexModelId));
		const auto transform = CalculatePxTransform(transformComponent);

		rigidbodyStaticConvexComponent.Body = PxCreateStatic(	*Physics,
																transform,
																geometry,
																*physicsMaterialComponent.Material,
																CalculateOffsetPxTransform(rigidbodyStaticConvexComponent));

		SetFiltersForComponent(	*rigidbodyStaticConvexComponent.Body,
								physicsMaterialComponent.SurfaceFilterType);

		Scene->addActor(*rigidbodyStaticConvexComponent.Body);
	});
}

void PhysicsSystem::InitializeRigidbodyDynamicConvexComponent()
{
	Logger::Debug("Preparing to initialize rigidbody dynamic convex...");

	Registry->view<TransformComponent, PhysicsMaterialComponent, RigidbodyDynamicConvexComponent, entt::tag<Tags::REQUIRES_REBUILD>>().each(
		[this](TransformComponent &transformComponent, PhysicsMaterialComponent &physicsMaterialComponent, RigidbodyDynamicConvexComponent &rigidbodyDynamicConvexComponent, auto _)
	{
		const auto geometry = physx::PxConvexMeshGeometry(ModelLoader::GetConvexResource(rigidbodyDynamicConvexComponent.ConvexModelId));
		const auto transform = CalculatePxTransform(transformComponent);

		rigidbodyDynamicConvexComponent.Body = PxCreateDynamic(	*Physics,
																transform,
																geometry,
																*physicsMaterialComponent.Material,
																rigidbodyDynamicConvexComponent.Density,
																CalculateOffsetPxTransform(rigidbodyDynamicConvexComponent));

		SetFiltersForComponent(	*rigidbodyDynamicConvexComponent.Body,
								physicsMaterialComponent.SurfaceFilterType);

		Scene->addActor(*rigidbodyDynamicConvexComponent.Body);
	});
}

void PhysicsSystem::InitializeRigidbodyStaticHeightFields()
{
	Logger::Debug("Preparing to initialize rigidbody static height fields...");

	Registry->view<TransformComponent, PhysicsMaterialComponent, TerrainComponent, RigidbodyStaticHeightFieldsComponent, entt::tag<Tags::REQUIRES_REBUILD>>().each(
		[this](TransformComponent &transformComponent, PhysicsMaterialComponent &physicsMaterialComponent, TerrainComponent &terrainComponent, 
			RigidbodyStaticHeightFieldsComponent &rigidbodyStaticHeightFieldsComponent, auto _)
	{
		const auto geometry = TerrainUtil::GenerateTerrainForPhysx(	rigidbodyStaticHeightFieldsComponent.HeightFieldSample, 
																	rigidbodyStaticHeightFieldsComponent.HeightField,
																	Cooking, 
																	Physics, 
																	&Allocator, 
																	terrainComponent, 
																	physx::PxHeightFieldFormat::eS16_TM,
																	ConfigurationData->EnableAsyncTerrainGeneration);

		const auto transform = CalculatePxTransform(transformComponent);
		const auto actor = Physics->createRigidStatic(transform);

		rigidbodyStaticHeightFieldsComponent.Shape = physx::PxRigidActorExt::createExclusiveShape(*actor, geometry, *physicsMaterialComponent.Material);

		SetFiltersForComponent(	*rigidbodyStaticHeightFieldsComponent.Shape,
								physicsMaterialComponent.SurfaceFilterType);

		Scene->addActor(*actor);
	});
}

void PhysicsSystem::InitializeRoadComponent()
{
	Registry->view<TransformComponent, PhysicsMaterialComponent, RoadComponent, entt::tag<Tags::REQUIRES_REBUILD>>().each(
		[this](TransformComponent &transformComponent, PhysicsMaterialComponent &physicsMaterialComponent, RoadComponent &roadComponent, auto _)
	{
		PX_RELEASE(roadComponent.PhysicsBody);

		physx::PxTriangleMeshDesc meshDesc;
		meshDesc.points.count = roadComponent.VertexPositions.size();
		meshDesc.points.stride = sizeof(DirectX::XMFLOAT3);
		meshDesc.points.data = roadComponent.VertexPositions.data();
		meshDesc.triangles.count = roadComponent.Indices.size() / 3;
		meshDesc.triangles.stride = 3 * sizeof(DWORD);
		meshDesc.triangles.data = roadComponent.Indices.data();

		physx::PxDefaultMemoryOutputStream writeBuffer;
		physx::PxTriangleMeshCookingResult::Enum result;

		const auto status = Cooking->cookTriangleMesh(meshDesc, writeBuffer, &result);
		if (!status)
			Logger::Error("Cooking road failed!");

		switch (result)
		{
		case physx::PxTriangleMeshCookingResult::eSUCCESS:
			Logger::Debug("Cooking road mesh success!");
		case physx::PxTriangleMeshCookingResult::eLARGE_TRIANGLE:
			Logger::Warning("Cooking road finished with too large triangles!");
		case physx::PxTriangleMeshCookingResult::eFAILURE:
				Logger::Warning("Cooking road failed with unknown error!");
		}

		physx::PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
		const auto triangleMesh = Physics->createTriangleMesh(readBuffer);

		const auto geometry = physx::PxTriangleMeshGeometry(triangleMesh);
		const auto transform = CalculatePxTransform(transformComponent);

		roadComponent.PhysicsBody = PxCreateStatic(	*Physics,
													transform,
													geometry,
													*physicsMaterialComponent.Material,
													CalculateOffsetPxTransform(roadComponent));

		SetFiltersForComponent(	*roadComponent.PhysicsBody,
								physicsMaterialComponent.SurfaceFilterType);

		Scene->addActor(*roadComponent.PhysicsBody);
	});
}

void PhysicsSystem::InitializeVehiclesAndWheels()
{
	Logger::Debug("Preparing to initialize vehicles and wheels...");

	AssociateWheelsWithVehicles();
	VerifyWheelsForEachVehicle();
	CreateVehicle();
}

void PhysicsSystem::SetFiltersForComponent(physx::PxRigidActor& actor, const std::string& type) const
{
	physx::PxFilterData queryFilterData;

	PhysicsFilterHelper::SetSurfaceFilter(queryFilterData, type);
	PhysicsFilterHelper::SetQueryFilterDataForActor(actor, queryFilterData);

	physx::PxFilterData filterData;

	PhysicsFilterHelper::SetSimulationFilter(filterData, CollisionVehicleObstacle, CollisionVehicleObstacleAgainst);
	PhysicsFilterHelper::SetSimulationFilterDataForActor(actor, filterData);
}

void PhysicsSystem::SetFiltersForComponent(physx::PxShape& shape, const std::string& type) const
{
	physx::PxFilterData queryFilterData;

	PhysicsFilterHelper::SetSurfaceFilter(queryFilterData, type);
	shape.setQueryFilterData(queryFilterData);

	physx::PxFilterData filterData;

	PhysicsFilterHelper::SetSimulationFilter(filterData, CollisionVehicleObstacle, CollisionVehicleObstacleAgainst);
	shape.setSimulationFilterData(filterData);
}

void PhysicsSystem::UpdateSimulation() const
{
	Scene->simulate(ConfigurationData->PhysicsDeltaTime / 1000.0f);
	Scene->fetchResults(true);
}

void PhysicsSystem::UpdateMatrices() const
{
	UpdateMatrixFromRigidbody<RigidbodyDynamicBoxComponent>();
	UpdateMatrixFromRigidbody<RigidbodyDynamicSphereComponent>();
	UpdateMatrixFromRigidbody<RigidbodyDynamicCapsuleComponent>();
	UpdateMatrixFromRigidbody<RigidbodyDynamicCylinderComponent>();
}

void PhysicsSystem::UpdateVehicles() const
{
	std::vector<physx::PxVehicleWheels*> vehicles;
	std::vector<TransformComponent*> transformComponents;
	std::vector<VehicleComponent*> vehicleComponents;

	Registry->view<TransformComponent, PhysicsMaterialComponent, VehicleComponent>().each(
		[&vehicles, &transformComponents, &vehicleComponents](TransformComponent &transformComponentVehicle,
			PhysicsMaterialComponent &physicsMaterialComponentVehicle, VehicleComponent &vehicleComponent)
	{
		vehicles.emplace_back(vehicleComponent.Vehicle);
		transformComponents.emplace_back(&transformComponentVehicle);
		vehicleComponents.emplace_back(&vehicleComponent);
	});

	const auto sweepResults = VehicleSceneQueryData->GetSweepResultBuffer(0);
	const auto sweepResultsSize = VehicleSceneQueryData->GetSweepResultSize();
	PxVehicleSuspensionSweeps(	BatchQuery, 
								vehicles.size(), 
								vehicles.data(), 
								sweepResultsSize, 
								sweepResults, 
								ConfigurationData->VehicleHitsPerQuery,
								nullptr, 
								ConfigurationData->VehicleSweepWidthScale, 
								ConfigurationData->VehicleSweepRadiusScale);

	const auto gravity = Scene->getGravity();;
	std::vector<physx::PxWheelQueryResult> wheelQueryResults(4 * vehicles.size());

	std::vector<physx::PxVehicleWheelQueryResult> vehicleQueryResults(vehicles.size());
	for (auto i = 0; i < vehicles.size(); i++)
	{
		vehicleQueryResults[i] = { &wheelQueryResults[i * 4], vehicles[i]->mWheelsSimData.getNbWheels() };
	}

	PxVehicleUpdates(	ConfigurationData->PhysicsDeltaTime / 1000.0f,
						gravity,
						*FrictionPairs, 
						vehicles.size(), 
						vehicles.data(), 
						vehicleQueryResults.data());

	for (auto i = 0; i < vehicles.size(); i++)
	{
		const auto rigidActor = vehicles[i]->getRigidDynamicActor();

		if (rigidActor->isSleeping())
			continue;

		const auto chassisGlobalPose = rigidActor->getGlobalPose();
		const physx::PxMat44 chassisMatrix(chassisGlobalPose);

		TransformLogic::SetMatrix(chassisMatrix.getTranspose(), *transformComponents[i]);

		for (auto w = 0; w < std::size(vehicleComponents[i]->Wheels); w++)
		{
			auto wheelLocalPose = wheelQueryResults[i * 4 + w].localPose;
			if (vehicleComponents[i]->Wheels[w]->Flip)
				wheelLocalPose = wheelLocalPose.transform(physx::PxTransform(physx::PxQuat(physx::PxPi, physx::PxVec3(0.0f, 1.0f, 0.0f))));

			const auto wheelMatrix = chassisMatrix * physx::PxMat44(wheelLocalPose);

			TransformLogic::SetMatrix(wheelMatrix.getTranspose(), *vehicleComponents[i]->WheelTransform[w]);
		}
	}
}

physx::PxTransform PhysicsSystem::CalculatePxTransform(const TransformComponent& transformComponent) const
{
	DirectX::XMFLOAT4X4 matrixFloats{};
	XMStoreFloat4x4(&matrixFloats, transformComponent.WorldMatrix);

	const auto physicsMatrix = physx::PxMat44(
		physx::PxVec4(matrixFloats._11, matrixFloats._12, matrixFloats._13, matrixFloats._14),
		physx::PxVec4(matrixFloats._21, matrixFloats._22, matrixFloats._23, matrixFloats._24),
		physx::PxVec4(matrixFloats._31, matrixFloats._32, matrixFloats._33, matrixFloats._34),
		physx::PxVec4(matrixFloats._41, matrixFloats._42, matrixFloats._43, matrixFloats._44)
	);

	return physx::PxTransform(physicsMatrix);
}

void PhysicsSystem::AssociateWheelsWithVehicles()
{
	Logger::Debug("Preparing to associate wheels with vehicles by id...");

	Registry->view<TransformComponent, PhysicsMaterialComponent, WheelComponent, entt::tag<Tags::REQUIRES_REBUILD>>().each(
		[this](TransformComponent &transformComponentWheel, PhysicsMaterialComponent &physicsMaterialComponentWheel, WheelComponent &wheelComponent, auto _)
	{
		Registry->view<TransformComponent, PhysicsMaterialComponent, VehicleComponent, entt::tag<Tags::REQUIRES_REBUILD>>().each(
			[&wheelComponent, &physicsMaterialComponentWheel, &transformComponentWheel](TransformComponent &transformComponentVehicle,
				PhysicsMaterialComponent &physicsMaterialComponentVehicle, VehicleComponent &vehicleComponent, auto _)
		{
			if (wheelComponent.VehicleId == vehicleComponent.VehicleId)
			{
				Logger::Debug("Found vehicle for wheel!");

				if (wheelComponent.WheelId > 4 || wheelComponent.WheelId < 0)
				{
					Logger::Error("Wheel outside of range 0-4! Only four wheels vehicle are supported!");
				}
				else
				{
					Logger::Debug("Connecting wheel " + std::to_string(wheelComponent.WheelId) + " to vehicle " + wheelComponent.VehicleId + "...");
					vehicleComponent.Wheels[wheelComponent.WheelId] = &wheelComponent;
					vehicleComponent.WheelsMaterials[wheelComponent.WheelId] = &physicsMaterialComponentWheel;
					vehicleComponent.WheelTransform[wheelComponent.WheelId] = &transformComponentWheel;
				}
			}
		});
	});
}

void PhysicsSystem::VerifyWheelsForEachVehicle() const
{
	Logger::Debug("Checking if each vehicle has got all four wheels assigned...");

	Registry->view<VehicleComponent, entt::tag<Tags::REQUIRES_REBUILD>>().each([](VehicleComponent &vehicleComponent, auto _)
	{
		const auto size = std::size(vehicleComponent.Wheels);

		for (auto i = 0; i < size; i++)
		{
			if (vehicleComponent.Wheels[i] == nullptr || vehicleComponent.WheelsMaterials[i] == nullptr)
			{
				Logger::Error("Wheel with id " + std::to_string(i) + " not assigned to vehicle " + vehicleComponent.VehicleId + "!");
			}
		}
	});
}

void PhysicsSystem::CreateVehicle()
{
	Logger::Debug("Preparing to create physics vehicles...");

	Registry->view<TransformComponent, PhysicsMaterialComponent, VehicleComponent, entt::tag<Tags::REQUIRES_REBUILD>>().each(
		[this](TransformComponent &transformComponent, PhysicsMaterialComponent &physicsMaterialComponent, VehicleComponent &vehicleComponent, auto _)
	{
		const auto vehicle = VehicleResourcesGenerator::Create4WheelVehicle(Physics, 
																			Cooking, 
																			vehicleComponent, 
																			physicsMaterialComponent,
																			transformComponent);

		if (!vehicle)
		{
			Logger::Error("Vehicle couldn't be created!");
			return;
		}

		vehicleComponent.Vehicle = vehicle;

		for (auto currentWheel : vehicleComponent.Wheels)
			currentWheel->Vehicle = vehicle;

		Scene->addActor(*vehicle->getRigidDynamicActor());

		vehicle->setToRestState();
		vehicle->mDriveDynData.forceGearChange(physx::PxVehicleGearsData::eFIRST);
		vehicle->mDriveDynData.setUseAutoGears(vehicleComponent.GearboxType == "AUTOMATIC");
	});
}
