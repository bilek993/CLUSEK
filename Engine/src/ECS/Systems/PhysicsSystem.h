#pragma once
#include "BaseSystem.h"
#include "../../Physics/PhysicsErrorCallback.h"
#include "PxPhysicsAPI.h"

#define PX_RELEASE(x) if(x) { x->release(); x = nullptr; }

struct TransformComponent;

class PhysicsSystem final : public BaseSystem
{
public:
	void Start() override;
	void Update(float deltaTime) override;
	~PhysicsSystem() override;
private:
	void InitializeCore();

	void InitializePhysicsMaterialComponents();
	void InitializeRigidbodyStaticPlaneComponents();
	void InitializeRigidbodyStaticBoxComponents();
	void InitializeRigidbodyDynamicBoxComponents();

	physx::PxTransform CalculatePxTransform(const TransformComponent &transformComponent) const;

	physx::PxDefaultAllocator Allocator;
	PhysicsErrorCallback ErrorCallback;

	physx::PxFoundation* Foundation = nullptr;
	physx::PxPhysics* Physics = nullptr;

	physx::PxDefaultCpuDispatcher* Dispatcher = nullptr;
	physx::PxScene* Scene = nullptr;

	physx::PxPvd* Pvd = nullptr;

	float TimeSum = 0.0f;
};
