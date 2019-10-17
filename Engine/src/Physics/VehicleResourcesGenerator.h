#pragma once
#include <PxPhysicsAPI.h>
#include "../ECS/Components/VehicleComponent.h"

class VehicleResourcesGenerator final
{
public:
	static physx::PxVehicleDrive4W* Create4WheelVehicle(physx::PxPhysics* physics, const physx::PxCooking* cooking,
		const VehicleComponent& vehicleComponent, const PhysicsMaterialComponent& vehicleMaterialComponent);
private:
	static physx::PxRigidDynamic* Create4WheelVehicleActor(physx::PxPhysics* physics, const physx::PxCooking* cooking,
		const VehicleComponent& vehicleComponent, const PhysicsMaterialComponent& vehicleMaterialComponent);
};
