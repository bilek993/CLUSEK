#pragma once
#include "BaseSystem.h"

class SkyboxSystem final : public BaseSystem
{
public:
	void Start() override;
	void Update(float deltaTime) override;
};
