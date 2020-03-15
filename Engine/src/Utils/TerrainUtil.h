#pragma once
#include "../ECS/Components/TerrainComponent.h"
#include <vector>
#include <stb_image.h>
#include <geometry/PxHeightFieldSample.h>
#include <geometry/PxHeightField.h>
#include <extensions/PxDefaultAllocator.h>
#include <cooking/PxCooking.h>
#include <geometry/PxHeightFieldGeometry.h>
#include "../Renderer/ConstantBufferTypes/TerrainNormalBuffer.h"
#include "../Renderer/ConstantBuffer.h"

class TerrainUtil final
{
public:
	static void GenerateTerrainMesh(TerrainComponent& terrainComponent, ID3D11Device* device, bool async);
	static physx::PxHeightFieldGeometry GenerateTerrainForPhysx(physx::PxHeightFieldSample* heightFieldSample, physx::PxHeightField* heightField,
		physx::PxCooking* cooking, physx::PxPhysics* physics, physx::PxDefaultAllocator* allocator, const TerrainComponent& terrainComponent, 
		physx::PxHeightFieldFormat::Enum format, bool async);

	static void OptimizeTerrain(TerrainComponent& terrainComponent, ID3D11Device* device, ID3D11DeviceContext* context);
	static void GenerateNormals(TerrainComponent& terrainComponent, ConstantBuffer<TerrainNormalBuffer>& terrainNormalBuffer, 
		ID3D11Device* device, ID3D11DeviceContext* context);

private:
	inline static const int THREAD_COUNT = 32;

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

	static void InitializeOptimizationTextures(TerrainComponent& terrainComponent, int width, int height, ID3D11Device* device);
	static bool CalculateOptimizedTexturesSize(TerrainComponent& terrainComponent, int* width, int* height);
	static void ComputeTexturesOptimization(TerrainComponent& terrainComponent, int width, int height, ComputeShader& computeShader, ID3D11DeviceContext* context);
	static void GenerateMipMapsForOptimizedTextures(TerrainComponent& terrainComponent, ID3D11DeviceContext* context);
	static void CleanUpResourcesAfterOptimization(TerrainComponent& terrainComponent, ID3D11DeviceContext* context);

	static D3D11_TEXTURE2D_DESC GetDescriptor(ID3D11ShaderResourceView* shaderResourceView);
};
