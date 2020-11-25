#pragma once
#include <mutex>

#include "../ECS/Components/RoadComponent.h"
#include "../Renderer/Mesh.h"

class RoadMeshGenerator final
{
public:
	static void RegenerateRoadComponent(ID3D11Device* device, RoadComponent &roadComponent, bool async);
private:
	static void ClearOldComponentData(RoadComponent &roadComponent);
	static void GenerateSupportPoints(RoadComponent &roadComponent, bool async);

	static void CalculatePartialLookUpTableAndInsertThem(RoadComponent* roadComponent, int firstPointId);
	
	static void GenerateRoadMesh(ID3D11Device* device, RoadComponent &roadComponent, bool async);
	
	static void GenerateVertices(ID3D11Device* device, RoadComponent* roadComponent);
	static void GenerateIndices(ID3D11Device* device, RoadComponent* roadComponent);

	static DirectX::XMVECTOR CalculateTangent(const DirectX::XMVECTOR* previousPoint, 
		const DirectX::XMVECTOR* currentPoint, const DirectX::XMVECTOR* nextPoint);
	static DirectX::XMVECTOR CalculateBitangent(const DirectX::XMVECTOR& tangent);
	static DirectX::XMVECTOR CalculateNormal(const DirectX::XMVECTOR& tangent, const DirectX::XMVECTOR& bitangent);

	static DirectX::XMVECTOR CalculatePosition(const DirectX::XMVECTOR& tangent, const DirectX::XMVECTOR& bitangent,
		const DirectX::XMVECTOR& normal, DirectX::XMFLOAT2 meshVertex, const DirectX::XMVECTOR& currentPoint);
	static DirectX::XMFLOAT2 CalculateTextureCoord(int vertexId, float totalSegmentWidth, float& distanceSinceStart,
		float textureScaling, const DirectX::XMVECTOR* previousPoint, const DirectX::XMVECTOR* currentPoint,
		const std::vector<DirectX::XMFLOAT2>& meshVertices);

	static float CalculateSegmentWidth(const std::vector<DirectX::XMFLOAT2>& meshVertices, int lastElement = -1);

	inline static std::mutex LookUpTableInsertLock{};
};
