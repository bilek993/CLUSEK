#pragma once
#include "../ECS/Components/TerrainComponent.h"
#include <vector>
#include <stb_image.h>
#include <geometry/PxHeightFieldSample.h>
#include <geometry/PxHeightField.h>
#include <extensions/PxDefaultAllocator.h>
#include <cooking/PxCooking.h>
#include <geometry/PxHeightFieldGeometry.h>

class TerrainUtil final
{
public:
	static void GenerateTerrainMesh(TerrainComponent& terrainComponent, ID3D11Device* device, bool async);
	static physx::PxHeightFieldGeometry GenerateTerrainForPhysx(physx::PxHeightFieldSample* heightFieldSample, physx::PxHeightField* heightField,
		physx::PxCooking* cooking, physx::PxPhysics* physics, physx::PxDefaultAllocator* allocator, const TerrainComponent& terrainComponent, 
		physx::PxHeightFieldFormat::Enum format, bool async);

private:
	static stbi_us* OpenFile(const std::string& path, int *width, int *height, int *numberOfChannels);

	static std::vector<PositionAndUvVertex> GenerateVertices(int width, int height, int numberOfChannels, 
		int qualityDivider, float scaleXZ, float maxHeight, stbi_us* data);
	static std::vector<DWORD> GenerateIndices(int width, int height, int qualityDivider);

	static void CalculatePartOfHeightField(int startX, int endX, int startY, int endY, int width,
		int numberOfChannels, stbi_us* data, physx::PxHeightFieldSample* heightFieldSample);

	static void CalculateBoundsY(PositionAndUvVertex* vertex, stbi_us* data, int width, int numberOfChannels, 
		float maxHeight, int currentX, int currentY, int nextX, int nextY);

	static stbi_us* CalculateOffset(stbi_us* data, int x, int y, int width, int numberOfChannels);
	static float GetHeight(const stbi_us* offsetData, float maxHeight);
};
