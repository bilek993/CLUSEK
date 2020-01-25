#pragma once
#include "BaseSystem.h"

class TerrainSystem final : public BaseSystem
{
public:
	void Start() override;
	void Update(float deltaTime) override;
};
