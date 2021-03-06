#pragma once
#include "PxPhysicsAPI.h"
#include "WheelComponent.h"
#include "PhysicsMaterialComponent.h"
#include <string>
#include "TransformComponent.h"

struct VehicleComponent final
{
	std::string VehicleId;
	float Mass;
	std::string ConvexModelId;
	DirectX::XMFLOAT3 ConvexPositionOffsetVector;
	DirectX::XMFLOAT3 ConvexRotationOffsetVector;
	float DimensionX;
	float DimensionY;
	float DimensionZ;
	float CenterOfMassOffsetX = 0.0f;
	float CenterOfMassOffsetY = 0.0f;
	float CenterOfMassOffsetZ = 0.0f;
	float MaxBrakeTorque;
	float MaxHandBrakeTorque;
	float MaxSteer = physx::PxPi * 0.3333f;
	float AntiRollBarStiffness = 10000.0f;
	std::string DifferentialType = "LIMITED_SLIP_4W";
	float PeakTorque = 500.0f;
	float MaxOmega = 600.0f;
	float EngineMomentOfInertia = 1.0f;
	float GearsSwitchTime = 0.85f;
	std::string GearboxType = "AUTOMATIC";
	std::string ClutchAccuracyMode = "BEST_POSSIBLE";
	float ClutchStrength = 10.0f;
	float AckermannAccuracy = 1.0f;

	WheelComponent* Wheels[4];
	PhysicsMaterialComponent* WheelsMaterials[4];
	TransformComponent* WheelTransform[4];
	physx::PxVehicleDrive4W* Vehicle;
};
