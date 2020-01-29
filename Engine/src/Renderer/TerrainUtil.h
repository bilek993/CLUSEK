#pragma once
#include "../ECS/Components/TerrainComponent.h"

class TerrainUtil final
{
public:
	static void GenerateTerrainMesh(TerrainComponent& terrainComponent, ID3D11Device* device);
};
