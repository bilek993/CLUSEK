#pragma once
#include "BaseSystem.h"

class RebuildSystem final : public BaseSystem
{
public:
	void Start() override;
	void Rebuild() override;
	void Update(float deltaTime) override;
};
