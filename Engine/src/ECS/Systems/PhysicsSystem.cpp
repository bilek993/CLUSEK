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

	PhysXVisualDebugger = PxCreatePvd(*Foundation);

	const auto transport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	PhysXVisualDebugger->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);
#endif
}

void PhysicsSystem::Update(const float deltaTime)
{
	// TODO: Implement me!
}

PhysicsSystem::~PhysicsSystem()
{
	if (PhysXVisualDebugger)
	{
		auto transport = PhysXVisualDebugger->getTransport();

		PX_RELEASE(PhysXVisualDebugger);
		PX_RELEASE(transport);
	}

	PX_RELEASE(Foundation);
}
