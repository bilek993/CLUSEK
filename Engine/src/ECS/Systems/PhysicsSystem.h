#pragma once
#include "BaseSystem.h"
#include "../../Physics/PhysicsErrorCallback.h"
#include "PxPhysicsAPI.h"
#include "../../Physics/PhysicsUnitConversion.h"
#include "../../Renderer/TransformLogic.h"
#include "../../Physics/VehicleSceneQueryData.h"

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
	void InitializeRigidbodyStaticSphereComponents();
	void InitializeRigidbodyDynamicSphereComponents();
	void InitializeRigidbodyStaticCapsuleComponents();
	void InitializeRigidbodyDynamicCapsuleComponents();
	void InitializeRigidbodyStaticCylinderComponents();
	void InitializeRigidbodyDynamicCylinderComponents();
	void InitializeVehiclesAndWheels();

	void SetFiltersForComponent(physx::PxRigidActor& actor, const std::string& type) const;

	void UpdateSimulation() const;
	void UpdateMatrices() const;

	template<class T>
	void UpdateMatrixFromRigidbody() const;

	void UpdateVehicles() const;

	physx::PxTransform CalculatePxTransform(const TransformComponent &transformComponent) const;

	void AssociateWheelsWithVehicles();
	void VerifyWheelsForEachVehicle() const;
	void CreateVehicle();

	physx::PxDefaultAllocator Allocator;
	PhysicsErrorCallback ErrorCallback;

	physx::PxFoundation* Foundation = nullptr;
	physx::PxPhysics* Physics = nullptr;
	physx::PxCooking* Cooking = nullptr;

	physx::PxDefaultCpuDispatcher* Dispatcher = nullptr;
	physx::PxScene* Scene = nullptr;

	physx::PxPvd* Pvd = nullptr;

	VehicleSceneQueryData* VehicleSceneQueryData = nullptr;
	physx::PxBatchQuery* BatchQuery = nullptr;

	physx::PxVehicleDrivableSurfaceToTireFrictionPairs* FrictionPairs = nullptr;

	float TimeSum = 0.0f;
};

template <class T>
void PhysicsSystem::UpdateMatrixFromRigidbody() const
{
	Registry->view<TransformComponent, T>().each([](TransformComponent &transformComponent, T &rigidbodyComponent)
	{
		const auto body = rigidbodyComponent.Body;
		if (!body->isSleeping())
		{
			const auto globalPose = body->getGlobalPose();
			const auto position = globalPose.p;
			const auto rotation = PhysicsUnitConversion::PhysicsQuaternionToDirectEuler(globalPose.q);

			TransformLogic::SetPosition(position.x, position.y, position.z, transformComponent);
			TransformLogic::SetRotation(rotation.x, rotation.y, rotation.z, transformComponent);
		}
	});
}
