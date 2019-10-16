#pragma once
#include "PxPhysicsAPI.h"
#include "WheelComponent.h"
#include "PhysicsMaterialComponent.h"

struct VehicleComponent final
{
	std::string VehicleId;

	WheelComponent* Wheels[4];
	PhysicsMaterialComponent* WheelsMaterials[4];
	physx::PxVehicleDrive4W* Vehicle;
};
