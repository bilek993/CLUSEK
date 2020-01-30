#pragma once
#include "../ECS/Components/TerrainComponent.h"
#include <vector>
#include <stb_image.h>

class TerrainUtil final
{
public:
	static void GenerateTerrainMesh(TerrainComponent& terrainComponent, ID3D11Device* device);

private:
	static std::vector<PositionAndUvVertex> GenerateVertices(int width, int height, int numberOfChannels, 
		int qualityDivider, float scaleXZ, float maxHeight, const stbi_us* data);
	static std::vector<DWORD> GenerateIndices(int width, int height, int qualityDivider);
};
