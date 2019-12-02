#pragma once
#include "BaseSystem.h"

class VehiclePlayerControllerSystem final : public BaseSystem
{
public:
	void Start() override;
	void Update(float deltaTime) override;
private:
	void HandleGamepad(float& accelerate, float& brake, float& left, float& handbrake, bool& changeToOrFromReverse) const;
	void HandleKeyboard(float& accelerate, float& brake, float& left, float& handbrake, bool& changeToOrFromReverse) const;

	float WheelAngel = 0.0f;
};
