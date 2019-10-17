#pragma once
#include <string>
#include "PxPhysicsAPI.h"

struct WheelComponent final
{
	std::string VehicleId;
	int WheelId;
	float Mass;
	float Width;
	float Radius;
};