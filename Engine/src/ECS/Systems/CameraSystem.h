#pragma once
#include "BaseSystem.h"
#include "../Components/CameraComponent.h"

class CameraSystem final : public BaseSystem
{
public:
	void Start() override;
	void Update(float deltaTime) override;
};
