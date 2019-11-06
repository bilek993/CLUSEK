#pragma once
#include <string>

struct WheelComponent final
{
	std::string VehicleId;
	int WheelId;
	float Mass;
	float Width;
	float Radius;
	int TireType = 0;
	float SuspensionMaxCompression = 0.3f;
	float SuspensionMaxDroop = 0.1f;
	float SuspensionSpringStrength;
	float SuspensionSpringDamperRate;
	float CamberAngleAtRest = 0.0f;
	float CamberAngleAtMaxDroop = 0.01f;
	float CamberAngleAtMaxCompression = -0.01f;
	float SuspensionForceApplicationPoinOffset = -0.3f;
	float TireForceApplicationPoinOffset = -0.3f;
	float LatStiffXMultiplier = 1.0f;
	float LatStiffYMultiplier = 1.0f;
};