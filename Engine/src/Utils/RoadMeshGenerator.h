#pragma once
#include "../ECS/Components/RoadComponent.h"
#include "../Renderer/Mesh.h"

class RoadMeshGenerator final
{
public:
	static void GenerateRoadMesh(ID3D11Device* device, RoadComponent &roadComponent);

private:
	static void GenerateVertices(ID3D11Device* device, RoadComponent &roadComponent);
	static void GenerateIndices(ID3D11Device* device, RoadComponent &roadComponent);

	static DirectX::XMVECTOR CalculateDirectionVector(const DirectX::XMVECTOR& currentPoint, const DirectX::XMVECTOR& nextPoint, 
		bool normalizeResult);
	static DirectX::XMVECTOR CalculateBitangent(const DirectX::XMVECTOR& tangent);
	static DirectX::XMVECTOR CalculateNormal(const DirectX::XMVECTOR& tangent, const DirectX::XMVECTOR& bitangent);
};
