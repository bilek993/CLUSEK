#pragma once
#include "BaseSystem.h"


class TransformSystem final : public BaseSystem
{
public:
	void Start() override;
	void Update(float deltaTime) override;
};