#include "PhysicsSystem.h"
#include "../Components/PhysicsMaterialComponent.h"
#include "../Components/RigidbodyStaticPlaneComponent.h"
#include "../Components/RigidbodyStaticBoxComponent.h"
#include "../Components/TransformComponent.h"
#include "../../Renderer/TransformLogic.h"
#include "../../Physics/PhysicsUnitConversion.h"
#include "../Components/RigidbodyDynamicBoxComponent.h"
#include "../Components/RigidbodyStaticSphereComponent.h"
#include "../Components/RigidbodyDynamicSphereComponent.h"
#include <thread>
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
#include "../Components/RigidbodyStaticCylinderComponent.h"

void PhysicsSystem::Start()
{
	Logger::Debug("Staring physics system...");

	InitializeCore();

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

PhysicsSystem::~PhysicsSystem()
{
	MaterialManager->ReleaseAllMaterials();

	const auto numberOfActors = Scene->getNbActors(physx::PxActorTypeFlag::eRIGID_DYNAMIC | physx::PxActorTypeFlag::eRIGID_STATIC);
	std::vector<physx::PxRigidActor*> actors(numberOfActors);
	Scene->getActors(physx::PxActorTypeFlag::eRIGID_DYNAMIC | physx::PxActorTypeFlag::eRIGID_STATIC, 
		reinterpret_cast<physx::PxActor**>(&actors[0]), numberOfActors);

	for (auto actor : actors)
		PX_RELEASE(actor);

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

	Foundation = PxCreateFoundation(PX_PHYSICS_VERSION, Allocator, ErrorCallback);
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

void PhysicsSystem::InitializePhysicsMaterialComponents()
{
	Logger::Debug("Preparing to initialize physics material...");

	MaterialManager = std::make_unique<PhysicsMaterialManager>(	Physics,
																ConfigurationData->DefaultPhysicsMaterialStaticFriction,
																ConfigurationData->DefaultPhysicsMaterialDynamicFriction,
																ConfigurationData->DefaultPhysicsMaterialRestitution);

	PhysicsMaterialLoader::Load(ConfigurationData->PathToPhysicsMaterials, MaterialManager.get(), Physics);

	Registry->view<PhysicsMaterialComponent>().each([this](PhysicsMaterialComponent &physicsMaterialComponent)
	{
		physicsMaterialComponent.Material = MaterialManager->GetMaterialByName(physicsMaterialComponent.Name);
		Logger::Debug("Paired material '" + physicsMaterialComponent.Name + "' with proper component.");
	});

	FrictionPairs = VehicleResourcesGenerator::CreateFrictionPairs(*Physics, MaterialManager.get());
}

void PhysicsSystem::InitializeRigidbodyStaticPlaneComponents()
{
	Logger::Debug("Preparing to initialize rigidbody static planes...");

	Registry->view<PhysicsMaterialComponent, RigidbodyStaticPlaneComponent>().each(
		[this](PhysicsMaterialComponent &physicsMaterialComponent, RigidbodyStaticPlaneComponent &rigidbodyStaticPlaneComponent)
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

	Registry->view<TransformComponent, PhysicsMaterialComponent, RigidbodyStaticBoxComponent>().each(
		[this](TransformComponent &transformComponent,PhysicsMaterialComponent &physicsMaterialComponent, RigidbodyStaticBoxComponent &rigidbodyStaticBoxComponent)
	{
		const auto geometry = physx::PxBoxGeometry(rigidbodyStaticBoxComponent.Width / 2, rigidbodyStaticBoxComponent.Height / 2, rigidbodyStaticBoxComponent.Depth / 2);
		const auto transform = CalculatePxTransform(transformComponent);

		rigidbodyStaticBoxComponent.Body = PxCreateStatic(	*Physics, 
															transform,
															geometry,
															*physicsMaterialComponent.Material);

		SetFiltersForComponent(	*rigidbodyStaticBoxComponent.Body, 
								physicsMaterialComponent.SurfaceFilterType);

		Scene->addActor(*rigidbodyStaticBoxComponent.Body);
	});
}

void PhysicsSystem::InitializeRigidbodyDynamicBoxComponents()
{
	Logger::Debug("Preparing to initialize rigidbody dynamic boxes...");

	Registry->view<TransformComponent, PhysicsMaterialComponent, RigidbodyDynamicBoxComponent>().each(
		[this](TransformComponent &transformComponent, PhysicsMaterialComponent &physicsMaterialComponent, RigidbodyDynamicBoxComponent &rigidbodyDynamicBoxComponent)
	{
		const auto geometry = physx::PxBoxGeometry(rigidbodyDynamicBoxComponent.Width / 2, rigidbodyDynamicBoxComponent.Height / 2, rigidbodyDynamicBoxComponent.Depth / 2);
		const auto transform = CalculatePxTransform(transformComponent);

		rigidbodyDynamicBoxComponent.Body = PxCreateDynamic(*Physics,
															transform,
															geometry,
															*physicsMaterialComponent.Material,
															rigidbodyDynamicBoxComponent.Density);

		SetFiltersForComponent(	*rigidbodyDynamicBoxComponent.Body,
								physicsMaterialComponent.SurfaceFilterType);

		Scene->addActor(*rigidbodyDynamicBoxComponent.Body);
	});
}

void PhysicsSystem::InitializeRigidbodyStaticSphereComponents()
{
	Logger::Debug("Preparing to initialize rigidbody static spheres...");

	Registry->view<TransformComponent, PhysicsMaterialComponent, RigidbodyStaticSphereComponent>().each(
		[this](TransformComponent &transformComponent, PhysicsMaterialComponent &physicsMaterialComponent, RigidbodyStaticSphereComponent &rigidbodyStaticSphereComponent)
	{
		const auto geometry = physx::PxSphereGeometry(rigidbodyStaticSphereComponent.Radius);
		const auto transform = CalculatePxTransform(transformComponent);

		rigidbodyStaticSphereComponent.Body = PxCreateStatic(	*Physics,
																transform,
																geometry,
																*physicsMaterialComponent.Material);

		SetFiltersForComponent(	*rigidbodyStaticSphereComponent.Body, 
								physicsMaterialComponent.SurfaceFilterType);

		Scene->addActor(*rigidbodyStaticSphereComponent.Body);
	});
}

void PhysicsSystem::InitializeRigidbodyDynamicSphereComponents()
{
	Logger::Debug("Preparing to initialize rigidbody dynamic spheres...");

	Registry->view<TransformComponent, PhysicsMaterialComponent, RigidbodyDynamicSphereComponent>().each(
		[this](TransformComponent &transformComponent, PhysicsMaterialComponent &physicsMaterialComponent, RigidbodyDynamicSphereComponent &rigidbodyDynamicSphereComponent)
	{
		const auto geometry = physx::PxSphereGeometry(rigidbodyDynamicSphereComponent.Radius);
		const auto transform = CalculatePxTransform(transformComponent);

		rigidbodyDynamicSphereComponent.Body = PxCreateDynamic(	*Physics,
																transform,
																geometry,
																*physicsMaterialComponent.Material,
																rigidbodyDynamicSphereComponent.Density);

		SetFiltersForComponent(	*rigidbodyDynamicSphereComponent.Body,
								physicsMaterialComponent.SurfaceFilterType);

		Scene->addActor(*rigidbodyDynamicSphereComponent.Body);
	});
}

void PhysicsSystem::InitializeRigidbodyStaticCapsuleComponents()
{
	Logger::Debug("Preparing to initialize rigidbody static capsules...");

	Registry->view<TransformComponent, PhysicsMaterialComponent, RigidbodyStaticCapsuleComponent>().each(
		[this](TransformComponent &transformComponent, PhysicsMaterialComponent &physicsMaterialComponent, RigidbodyStaticCapsuleComponent &rigidbodyStaticCapsuleComponent)
	{
		const auto geometry = physx::PxCapsuleGeometry(rigidbodyStaticCapsuleComponent.Radius, rigidbodyStaticCapsuleComponent.Height / 2);
		const auto transform = CalculatePxTransform(transformComponent);

		rigidbodyStaticCapsuleComponent.Body = PxCreateStatic(	*Physics,
																transform,
																geometry,
																*physicsMaterialComponent.Material);

		SetFiltersForComponent(	*rigidbodyStaticCapsuleComponent.Body, 
								physicsMaterialComponent.SurfaceFilterType);

		Scene->addActor(*rigidbodyStaticCapsuleComponent.Body);
	});
}

void PhysicsSystem::InitializeRigidbodyDynamicCapsuleComponents()
{
	Logger::Debug("Preparing to initialize rigidbody dynamic capsules...");

	Registry->view<TransformComponent, PhysicsMaterialComponent, RigidbodyDynamicCapsuleComponent>().each(
		[this](TransformComponent &transformComponent, PhysicsMaterialComponent &physicsMaterialComponent, RigidbodyDynamicCapsuleComponent &rigidbodyDynamicCapsuleComponent)
	{
		const auto geometry = physx::PxCapsuleGeometry(rigidbodyDynamicCapsuleComponent.Radius, rigidbodyDynamicCapsuleComponent.Height / 2);
		const auto transform = CalculatePxTransform(transformComponent);

		rigidbodyDynamicCapsuleComponent.Body = PxCreateDynamic(*Physics,
																transform,
																geometry,
																*physicsMaterialComponent.Material,
																rigidbodyDynamicCapsuleComponent.Density);

		SetFiltersForComponent(	*rigidbodyDynamicCapsuleComponent.Body, 
								physicsMaterialComponent.SurfaceFilterType);

		Scene->addActor(*rigidbodyDynamicCapsuleComponent.Body);
	});
}

void PhysicsSystem::InitializeRigidbodyStaticCylinderComponents()
{
	Logger::Debug("Preparing to initialize rigidbody static cylinders...");

	Registry->view<TransformComponent, PhysicsMaterialComponent, RigidbodyStaticCylinderComponent>().each(
		[this](TransformComponent &transformComponent, PhysicsMaterialComponent &physicsMaterialComponent, RigidbodyStaticCylinderComponent &rigidbodyStaticCylinderComponent)
	{
		const auto geometry = physx::PxConvexMeshGeometry(
			PhysicsMeshGenerator::CreateCylinder(*Physics, *Cooking, rigidbodyStaticCylinderComponent.Width, rigidbodyStaticCylinderComponent.Radius));
		const auto transform = CalculatePxTransform(transformComponent);

		rigidbodyStaticCylinderComponent.Body = PxCreateStatic(	*Physics,
																transform,
																geometry,
																*physicsMaterialComponent.Material);

		SetFiltersForComponent(	*rigidbodyStaticCylinderComponent.Body,
								physicsMaterialComponent.SurfaceFilterType);

		Scene->addActor(*rigidbodyStaticCylinderComponent.Body);
	});
}

void PhysicsSystem::InitializeRigidbodyDynamicCylinderComponents()
{
	Logger::Debug("Preparing to initialize rigidbody dynamic cylinders...");

	Registry->view<TransformComponent, PhysicsMaterialComponent, RigidbodyDynamicCylinderComponent>().each(
		[this](TransformComponent &transformComponent, PhysicsMaterialComponent &physicsMaterialComponent, RigidbodyDynamicCylinderComponent &rigidbodyDynamicCylinderComponent)
	{
		const auto geometry = physx::PxConvexMeshGeometry(
			PhysicsMeshGenerator::CreateCylinder(*Physics, *Cooking, rigidbodyDynamicCylinderComponent.Width, rigidbodyDynamicCylinderComponent.Radius));
		const auto transform = CalculatePxTransform(transformComponent);

		rigidbodyDynamicCylinderComponent.Body = PxCreateDynamic(*Physics,
																transform,
																geometry,
																*physicsMaterialComponent.Material,
																rigidbodyDynamicCylinderComponent.Density);

		SetFiltersForComponent(	*rigidbodyDynamicCylinderComponent.Body, 
								physicsMaterialComponent.SurfaceFilterType);

		Scene->addActor(*rigidbodyDynamicCylinderComponent.Body);
	});
}

void PhysicsSystem::InitializeRigidbodyStaticHeightFields()
{
	Logger::Debug("Preparing to initialize rigidbody static height fields...");

	Registry->view<TransformComponent, PhysicsMaterialComponent, TerrainComponent, RigidbodyStaticHeightFieldsComponent>().each(
		[this](TransformComponent &transformComponent, PhysicsMaterialComponent &physicsMaterialComponent, TerrainComponent &terrainComponent, 
			RigidbodyStaticHeightFieldsComponent &rigidbodyStaticHeightFieldsComponent)
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

	Registry->view<TransformComponent, PhysicsMaterialComponent, WheelComponent>().each(
		[this](TransformComponent &transformComponentWheel, PhysicsMaterialComponent &physicsMaterialComponentWheel, WheelComponent &wheelComponent)
	{
		Registry->view<TransformComponent, PhysicsMaterialComponent, VehicleComponent>().each(
			[&wheelComponent, &physicsMaterialComponentWheel, &transformComponentWheel](TransformComponent &transformComponentVehicle,
				PhysicsMaterialComponent &physicsMaterialComponentVehicle, VehicleComponent &vehicleComponent)
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

	Registry->view<VehicleComponent>().each([](VehicleComponent &vehicleComponent)
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

	Registry->view<TransformComponent, PhysicsMaterialComponent, VehicleComponent>().each(
		[this](TransformComponent &transformComponent, PhysicsMaterialComponent &physicsMaterialComponent, VehicleComponent &vehicleComponent)
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

		Scene->addActor(*vehicle->getRigidDynamicActor());

		vehicle->setToRestState();
		vehicle->mDriveDynData.forceGearChange(physx::PxVehicleGearsData::eFIRST);
		vehicle->mDriveDynData.setUseAutoGears(vehicleComponent.GearboxType == "AUTOMATIC");
	});
}
