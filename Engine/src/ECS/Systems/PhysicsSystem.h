#pragma once
#include "BaseSystem.h"
#include "../../Physics/PhysicsErrorCallback.h"
#include "PxPhysicsAPI.h"

#define PX_RELEASE(x) if(x) x->release(); x = NULL;

class PhysicsSystem final : public BaseSystem
{
public:
	void Start() override;
	void Update(float deltaTime) override;
	~PhysicsSystem() override;
private:
	physx::PxDefaultAllocator Allocator;
	PhysicsErrorCallback ErrorCallback;

	physx::PxFoundation* Foundation = nullptr;
	physx::PxPhysics* Physics = nullptr;

	physx::PxDefaultCpuDispatcher* Dispatcher = nullptr;
	physx::PxScene* Scene = nullptr;

	physx::PxPvd* PhysXVisualDebugger = nullptr;
};
