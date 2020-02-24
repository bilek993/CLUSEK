#pragma once
#include "../ECS/Components/TerrainComponent.h"
#include <vector>
#include <stb_image.h>

class TerrainUtil final
{
public:
	static void GenerateTerrainMesh(TerrainComponent& terrainComponent, ID3D11Device* device, bool async);

private:
	static std::vector<PositionAndUvVertex> GenerateVertices(int width, int height, int numberOfChannels, 
		int qualityDivider, float scaleXZ, float maxHeight, stbi_us* data);
	static std::vector<DWORD> GenerateRenderIndices(int width, int height, int qualityDivider);
	static std::vector<DWORD> GenerateShadowIndices(int width, int height, int qualityDivider);

	static void CalculateBoundsY(PositionAndUvVertex* vertex, stbi_us* data, int width, int numberOfChannels, 
		float maxHeight, int currentX, int currentY, int nextX, int nextY);

	static stbi_us* CalculateOffset(stbi_us* data, int x, int y, int width, int numberOfChannels);
	static float GetHeight(const stbi_us* offsetData, float maxHeight);
};
