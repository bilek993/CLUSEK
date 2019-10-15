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
#include "../../Loaders/Components/RigidbodyStaticCylinderComponent.h"
#include "../Components/RigidbodyDynamicCylinderComponent.h"

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
	InitializeVehicle();
}

void PhysicsSystem::Update(const float deltaTime)
{
	TimeSum += deltaTime;

	while (TimeSum >= ConfigurationData->PhysicsDeltaTime)
	{
		TimeSum -= ConfigurationData->PhysicsDeltaTime;
		UpdateSimulation();
		UpdateMatrices();
	}
}

PhysicsSystem::~PhysicsSystem()
{
	Registry->view<PhysicsMaterialComponent>().each([](PhysicsMaterialComponent &physicsMaterialComponent)
	{
		PX_RELEASE(physicsMaterialComponent.Material);
	});

	const auto numberOfActors = Scene->getNbActors(physx::PxActorTypeFlag::eRIGID_DYNAMIC | physx::PxActorTypeFlag::eRIGID_STATIC);
	std::vector<physx::PxRigidActor*> actors(numberOfActors);
	Scene->getActors(physx::PxActorTypeFlag::eRIGID_DYNAMIC | physx::PxActorTypeFlag::eRIGID_STATIC, 
		reinterpret_cast<physx::PxActor**>(&actors[0]), numberOfActors);

	for (auto actor : actors)
		PX_RELEASE(actor);

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
	sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
	Scene = Physics->createScene(sceneDesc);

	Cooking = PxCreateCooking(PX_PHYSICS_VERSION, *Foundation, physx::PxCookingParams(physx::PxTolerancesScale()));

#ifdef _DEBUG
	auto pvdClient = Scene->getScenePvdClient();;
	pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
	pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
	pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
#endif
}

void PhysicsSystem::InitializePhysicsMaterialComponents()
{
	Registry->view<PhysicsMaterialComponent>().each([this](PhysicsMaterialComponent &physicsMaterialComponent)
	{
		physicsMaterialComponent.Material = Physics->createMaterial(physicsMaterialComponent.StaticFriction, 
																	physicsMaterialComponent.DynamicFriction, 
																	physicsMaterialComponent.Restitution);
	});
}

void PhysicsSystem::InitializeRigidbodyStaticPlaneComponents()
{
	Registry->view<PhysicsMaterialComponent, RigidbodyStaticPlaneComponent>().each(
		[this](PhysicsMaterialComponent &physicsMaterialComponent, RigidbodyStaticPlaneComponent &rigidbodyStaticPlaneComponent)
	{
		rigidbodyStaticPlaneComponent.Body = PxCreatePlane(*Physics, physx::PxPlane(rigidbodyStaticPlaneComponent.NormalX, 
																					rigidbodyStaticPlaneComponent.NormalY, 
																					rigidbodyStaticPlaneComponent.NormalZ, 
																					rigidbodyStaticPlaneComponent.Distance), *physicsMaterialComponent.Material);
		Scene->addActor(*rigidbodyStaticPlaneComponent.Body);
	});
}

void PhysicsSystem::InitializeRigidbodyStaticBoxComponents()
{
	Registry->view<TransformComponent, PhysicsMaterialComponent, RigidbodyStaticBoxComponent>().each(
		[this](TransformComponent &transformComponent,PhysicsMaterialComponent &physicsMaterialComponent, RigidbodyStaticBoxComponent &rigidbodyStaticBoxComponent)
	{
		const auto geometry = physx::PxBoxGeometry(rigidbodyStaticBoxComponent.Width / 2, rigidbodyStaticBoxComponent.Height / 2, rigidbodyStaticBoxComponent.Depth / 2);
		const auto transform = CalculatePxTransform(transformComponent);

		rigidbodyStaticBoxComponent.Body = PxCreateStatic(	*Physics, 
															transform,
															geometry,
															*physicsMaterialComponent.Material);
		Scene->addActor(*rigidbodyStaticBoxComponent.Body);
	});
}

void PhysicsSystem::InitializeRigidbodyDynamicBoxComponents()
{
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
		Scene->addActor(*rigidbodyDynamicBoxComponent.Body);
	});
}

void PhysicsSystem::InitializeRigidbodyStaticSphereComponents()
{
	Registry->view<TransformComponent, PhysicsMaterialComponent, RigidbodyStaticSphereComponent>().each(
		[this](TransformComponent &transformComponent, PhysicsMaterialComponent &physicsMaterialComponent, RigidbodyStaticSphereComponent &rigidbodyStaticSphereComponent)
	{
		const auto geometry = physx::PxSphereGeometry(rigidbodyStaticSphereComponent.Radius);
		const auto transform = CalculatePxTransform(transformComponent);

		rigidbodyStaticSphereComponent.Body = PxCreateStatic(	*Physics,
																transform,
																geometry,
																*physicsMaterialComponent.Material);
		Scene->addActor(*rigidbodyStaticSphereComponent.Body);
	});
}

void PhysicsSystem::InitializeRigidbodyDynamicSphereComponents()
{
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
		Scene->addActor(*rigidbodyDynamicSphereComponent.Body);
	});
}

void PhysicsSystem::InitializeRigidbodyStaticCapsuleComponents()
{
	Registry->view<TransformComponent, PhysicsMaterialComponent, RigidbodyStaticCapsuleComponent>().each(
		[this](TransformComponent &transformComponent, PhysicsMaterialComponent &physicsMaterialComponent, RigidbodyStaticCapsuleComponent &rigidbodyStaticCapsuleComponent)
	{
		const auto geometry = physx::PxCapsuleGeometry(rigidbodyStaticCapsuleComponent.Radius, rigidbodyStaticCapsuleComponent.Height / 2);
		const auto transform = CalculatePxTransform(transformComponent);

		rigidbodyStaticCapsuleComponent.Body = PxCreateStatic(	*Physics,
																transform,
																geometry,
																*physicsMaterialComponent.Material);
		Scene->addActor(*rigidbodyStaticCapsuleComponent.Body);
	});
}

void PhysicsSystem::InitializeRigidbodyDynamicCapsuleComponents()
{
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
		Scene->addActor(*rigidbodyDynamicCapsuleComponent.Body);
	});
}

void PhysicsSystem::InitializeRigidbodyStaticCylinderComponents()
{
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
		Scene->addActor(*rigidbodyStaticCylinderComponent.Body);
	});
}

void PhysicsSystem::InitializeRigidbodyDynamicCylinderComponents()
{
	Registry->view<TransformComponent, PhysicsMaterialComponent, RigidbodyDynamicCylinderComponent>().each(
		[this](TransformComponent &transformComponent, PhysicsMaterialComponent &physicsMaterialComponent, RigidbodyDynamicCylinderComponent &rigidbodyDynamicCylinderComponent)
	{
		const auto geometry = physx::PxConvexMeshGeometry(
			PhysicsMeshGenerator::CreateCylinder(*Physics, *Cooking, rigidbodyDynamicCylinderComponent.Width, rigidbodyDynamicCylinderComponent.Radius));
		const auto transform = CalculatePxTransform(transformComponent);

		rigidbodyDynamicCylinderComponent.Body = PxCreateStatic(*Physics,
																transform,
																geometry,
																*physicsMaterialComponent.Material);
		Scene->addActor(*rigidbodyDynamicCylinderComponent.Body);
	});
}

void PhysicsSystem::InitializeVehicle()
{
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
}

physx::PxTransform PhysicsSystem::CalculatePxTransform(const TransformComponent& transformComponent) const
{
	const auto position = TransformLogic::GetPosition(transformComponent);
	const auto rotation = TransformLogic::GetRotation(transformComponent);

	return physx::PxTransform(	physx::PxVec3(position.x, position.y, position.z),
								PhysicsUnitConversion::DirectEulerToPhysicsQuaternion(rotation));
}
