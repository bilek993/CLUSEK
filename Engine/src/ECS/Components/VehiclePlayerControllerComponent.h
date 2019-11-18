#pragma once

struct VehiclePlayerControllerComponent final
{
	bool Possessed = false;
	float SteeringSpeed = 0.0075f;
	float WheelReturningToNeutralPosition = 0.0001f;
};