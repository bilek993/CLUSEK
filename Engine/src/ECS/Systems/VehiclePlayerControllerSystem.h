#pragma once
#include "BaseSystem.h"

class VehiclePlayerControllerSystem final : public BaseSystem
{
public:
	void Start() override;
	void Update(float deltaTime) override;
};
