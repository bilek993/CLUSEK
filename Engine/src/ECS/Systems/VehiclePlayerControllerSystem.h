#pragma once
#include "BaseSystem.h"
#include "../Components/VehicleComponent.h"
#include "../Components/VehiclePlayerControllerComponent.h"

class VehiclePlayerControllerSystem final : public BaseSystem
{
public:
	void Start() override;
	void Rebuild() override;
	void Update(float deltaTime) override;
private:
	void HandleGamepad(float& accelerate, float& brake, float& left, float& handbrake, 
		bool& changeToOrFromReverse, bool& gearUp, bool& gearDown) const;
	void HandleKeyboard(float& accelerate, float& brake, float& left, float& handbrake, 
		bool& changeToOrFromReverse, bool& gearUp, bool& gearDown) const;

	float CalculateVehicleSpeed(const VehicleComponent &vehicleComponent) const;
	float CalculateAndSetWheelAngle(float deltaTime, float left, float vehicleSpeed,
		float currentWheelAngle, const VehiclePlayerControllerComponent &vehiclePlayerControllerComponent) const;
	void HandleChangingFromOrToReverse(float vehicleSpeed, bool changeToOrFromReverse, 
		VehiclePlayerControllerComponent &vehiclePlayerControllerComponent, VehicleComponent &vehicleComponent) const;
};
