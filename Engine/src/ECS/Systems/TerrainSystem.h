#pragma once
#include "BaseSystem.h"
#include "../Components/TerrainComponent.h"

class TerrainSystem final : public BaseSystem
{
public:
	void Start() override;
	void Update(float deltaTime) override;

private:
	void GenerateTerrainMesh(TerrainComponent& terrainComponent) const;
};
