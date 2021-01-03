#pragma once

struct VehiclePlayerControllerComponent final
{
	bool Possessed = false;
	float SteeringSpeed = 0.0075f;
	float WheelReturningToNeutralPosition = 0.0001f;
	float SteeringLimiterFactor = 0.75f;
	float SteeringLimiterTopSpeed = 100.0f;
	float MinimalSpeedForGearChangeFromOrToReverse = 1.0f;

	bool Reverse = false;
};