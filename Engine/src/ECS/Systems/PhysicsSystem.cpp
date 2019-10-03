#include "PhysicsSystem.h"

void PhysicsSystem::Start()
{
	Logger::Debug("Staring physics system...");

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

	physx::PxSceneDesc sceneDesc(Physics->getTolerancesScale());
	sceneDesc.gravity = physx::PxVec3(ConfigurationData->GravityX, ConfigurationData->GravityY, ConfigurationData->GravityZ);
	Dispatcher = physx::PxDefaultCpuDispatcherCreate(2);
	sceneDesc.cpuDispatcher = Dispatcher;
	sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
	Scene = Physics->createScene(sceneDesc);

#ifdef _DEBUG
	auto pvdClient = Scene->getScenePvdClient();;
	pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
	pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
	pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
#endif
}

void PhysicsSystem::Update(const float deltaTime)
{
	Scene->simulate(deltaTime / 1000.0f);
	Scene->fetchResults(true);
}

PhysicsSystem::~PhysicsSystem()
{
	PX_RELEASE(Scene);
	PX_RELEASE(Dispatcher);
	PX_RELEASE(Physics);

	if (Pvd)
	{
		auto transport = Pvd->getTransport();

		PX_RELEASE(Pvd);
		PX_RELEASE(transport);
	}

	PX_RELEASE(Foundation);
}
