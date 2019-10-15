#pragma once
#include "PxPhysicsAPI.h"
#include "WheelComponent.h"

struct VehicleComponent final
{
	std::string VehicleId;

	WheelComponent* Wheels[4];
	physx::PxVehicleDrive4W* Vehicle;
};
