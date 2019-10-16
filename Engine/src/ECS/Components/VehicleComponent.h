#pragma once
#include "PxPhysicsAPI.h"
#include "WheelComponent.h"
#include "PhysicsMaterialComponent.h"
#include <string>

struct VehicleComponent final
{
	std::string VehicleId;
	float Mass;
	float DimensionX;
	float DimensionY;
	float DimensionZ;
	float CenterOfMassOffsetX = 0.0f;
	float CenterOfMassOffsetY = 0.0f;
	float CenterOfMassOffsetZ = 0.0f;

	WheelComponent* Wheels[4];
	PhysicsMaterialComponent* WheelsMaterials[4];
	physx::PxVehicleDrive4W* Vehicle;
};
