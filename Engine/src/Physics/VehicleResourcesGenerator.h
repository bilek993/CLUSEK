#pragma once
#include <PxPhysicsAPI.h>
#include "../ECS/Components/VehicleComponent.h"
#include <vector>
#include "PhysicsMaterialManager.h"

class VehicleResourcesGenerator final
{
public:
	static physx::PxVehicleDrive4W* Create4WheelVehicle(physx::PxPhysics* physics, const physx::PxCooking* cooking,
		const VehicleComponent& vehicleComponent, const PhysicsMaterialComponent& vehicleMaterialComponent,
		const TransformComponent& vehicleTransformComponent);

	static physx::PxVehicleDrivableSurfaceToTireFrictionPairs* CreateFrictionPairs(
		physx::PxPhysics& physics, PhysicsMaterialManager* physicsMaterialManager);

	static float CalculateWheelMOI(float mass, float radius);

private:
	static physx::PxRigidDynamic* Create4WheelVehicleActor(physx::PxPhysics* physics, const physx::PxCooking* cooking,
		const VehicleComponent& vehicleComponent, const PhysicsMaterialComponent& vehicleMaterialComponent,
		physx::PxConvexMesh* vehicleMesh, const physx::PxVehicleChassisData& chassisData, int wheelsCount);

	static std::vector<physx::PxVec3> GenerateWheelsOffsets(const TransformComponent& vehicleTransformComponent,
		const VehicleComponent& vehicleComponent, int wheelsCount);

	static physx::PxVehicleWheelsSimData* CreateWheelsSimData(const VehicleComponent& vehicleComponent, 
		int wheelsCount, const physx::PxVec3& vehicleCenterOfMassOffset, const std::vector<physx::PxVec3>& wheelsOffset);

	static physx::PxVehicleDriveSimData4W CreateDriveSimData(const VehicleComponent& vehicleComponent,
		const physx::PxVehicleWheelsSimData* wheelsSimData);

	static void SetInitialTransform(const TransformComponent& vehicleTransformComponent, physx::PxVehicleDrive4W* vehicle);
};
