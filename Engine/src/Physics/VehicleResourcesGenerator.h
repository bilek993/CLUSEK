#pragma once
#include <PxPhysicsAPI.h>
#include "../ECS/Components/VehicleComponent.h"
#include <vector>

class VehicleResourcesGenerator final
{
public:
	static physx::PxVehicleDrive4W* Create4WheelVehicle(physx::PxPhysics* physics, const physx::PxCooking* cooking,
		const VehicleComponent& vehicleComponent, const PhysicsMaterialComponent& vehicleMaterialComponent,
		const TransformComponent& vehicleTransformComponent);
private:
	static physx::PxRigidDynamic* Create4WheelVehicleActor(physx::PxPhysics* physics, const physx::PxCooking* cooking,
		const VehicleComponent& vehicleComponent, const PhysicsMaterialComponent& vehicleMaterialComponent,
		const physx::PxVec3& vehicleDimensions, const physx::PxVehicleChassisData& chassisData, int wheelsCount);

	static std::vector<physx::PxVec3> GenerateWheelsOffsets(const TransformComponent& vehicleTransformComponent,
		const VehicleComponent& vehicleComponent, int wheelsCount);

	static physx::PxVehicleWheelsSimData* CreateWheelsSimData(const VehicleComponent& vehicleComponent, 
		int wheelsCount, const physx::PxVec3& vehicleCenterOfMassOffset, const std::vector<physx::PxVec3>& wheelsOffset);
};
