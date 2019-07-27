#pragma once
#include "BaseSystem.h"

class RotationSystem final : public BaseSystem
{
public:
	void Start() override;
	void Update(float deltaTime) override;
};
