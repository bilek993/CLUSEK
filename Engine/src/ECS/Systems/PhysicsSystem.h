#pragma once
#include "BaseSystem.h"
#include "../../Physics/PhysicsErrorCallback.h"
#include "PxPhysicsAPI.h"
#include "../../Renderer/TransformLogic.h"
#include "../../Physics/VehicleSceneQueryData.h"
#include "../../Physics/PhysicsMaterialManager.h"

#define PX_RELEASE(x) if(x) { x->release(); x = nullptr; }

class PhysicsSystem final : public BaseSystem
{
public:
	PhysicsSystem() = default;

	void Start() override;
	void Update(float deltaTime) override;

	physx::PxSimulationStatistics GetStatistics() const;

	physx::PxPhysics* GetPointerToInternalPhysics() const;
	std::shared_ptr<PhysicsMaterialManager> GetPhysicsMaterialManagerSmartPointer() const;

	void UpdateFrictionPairs();

	PhysicsSystem(const PhysicsSystem& other) = delete;
	PhysicsSystem(PhysicsSystem&& other) noexcept = delete;
	PhysicsSystem& operator=(const PhysicsSystem& other) = delete;
	PhysicsSystem& operator=(PhysicsSystem&& other) noexcept = delete;

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
	void InitializeRigidbodyStaticHeightFields();
	void InitializeVehiclesAndWheels();

	void SetFiltersForComponent(physx::PxRigidActor& actor, const std::string& type) const;
	void SetFiltersForComponent(physx::PxShape& shape, const std::string& type) const;

	void UpdateSimulation() const;
	void UpdateMatrices() const;

	template<class T>
	void UpdateMatrixFromRigidbody() const;

	void UpdateVehicles() const;

	physx::PxTransform CalculatePxTransform(const TransformComponent &transformComponent) const;

	void AssociateWheelsWithVehicles();
	void VerifyWheelsForEachVehicle() const;
	void CreateVehicle();

	std::shared_ptr<PhysicsMaterialManager> MaterialManager;

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
			const physx::PxMat44 bodyMatrix(globalPose);

			TransformLogic::SetMatrix(bodyMatrix.getTranspose(), transformComponent);
		}
	});
}
