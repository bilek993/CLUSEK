#include "RoadMeshGenerator.h"

void RoadMeshGenerator::GenerateRoadMesh(ID3D11Device* device, RoadComponent& roadComponent)
{
	GenerateVertices(device, roadComponent);
	GenerateIndices(device, roadComponent);
}

void RoadMeshGenerator::GenerateVertices(ID3D11Device* device, RoadComponent& roadComponent)
{
	Logger::Debug("Generating road vertices...");

	const auto vertexCount = roadComponent.Points.size() * roadComponent.CalculatedSupportPoints.size();
	std::vector<FatVertex> vertices{};

	for (auto i = 0; i < roadComponent.CalculatedSupportPoints.size(); i++)
	{
		for (auto j = 0; j < roadComponent.MeshVertices.size(); j++)
		{
			const auto& currentPoint = roadComponent.CalculatedSupportPoints[i];
			const auto& nextPoint = roadComponent.CalculatedSupportPoints.size() >= (i + 1) ? XMLoadFloat3(&roadComponent.Points.back()) : roadComponent.CalculatedSupportPoints[i];
			
			const auto tangentVector = CalculateTangent(currentPoint, nextPoint);
			const auto bitangentVector = CalculateBitangent(tangentVector);
			const auto normalVector = CalculateNormal(tangentVector, bitangentVector);
			
			FatVertex vertex;

			//vertex.Position.x = mo; TODO: Add missing implementation
			//vertex.Position.y = mesh->mVertices[j].y; TODO: Add missing implementation
			//vertex.Position.z = mesh->mVertices[j].z; TODO: Add missing implementation

			XMStoreFloat3(&vertex.Normal, normalVector);

			XMStoreFloat3(&vertex.Tangent, tangentVector);

			//vertex.TextureCoord.x = static_cast<float>(mesh->mTextureCoords[0][j].x); TODO: Add missing implementation
			//vertex.TextureCoord.y = static_cast<float>(mesh->mTextureCoords[0][j].y); TODO: Add missing implementation
			
			vertices.emplace_back(vertex);
		}
	}

	roadComponent.Mesh.RenderVertexBuffer.Initialize(device, vertices.data(), vertexCount);
}

void RoadMeshGenerator::GenerateIndices(ID3D11Device* device, RoadComponent& roadComponent)
{
	Logger::Debug("Generating road indices...");
	
	const auto indicesCount = (roadComponent.Points.size() - 1) * 2;
	std::vector<DWORD> indices{};

	for (auto i = 0; i < indicesCount; i++)
	{
		const DWORD index = 0; // TODO: Add correct logic here
		indices.emplace_back(index);
	}

	roadComponent.Mesh.RenderIndexBuffer.Initialize(device, indices.data(), indicesCount);
}

DirectX::XMVECTOR RoadMeshGenerator::CalculateTangent(const DirectX::XMVECTOR& currentPoint, const DirectX::XMVECTOR& nextPoint)
{
	const auto directionVector = DirectX::XMVectorSubtract(nextPoint, currentPoint);
	const auto normalizedVector = DirectX::XMVector3Normalize(directionVector);
	return normalizedVector;
}

DirectX::XMVECTOR RoadMeshGenerator::CalculateBitangent(const DirectX::XMVECTOR& tangent)
{
	DirectX::XMFLOAT3 up(0.0f, 1.0f, 0.0f);
	const auto upVector = XMLoadFloat3(&up);

	const auto bitangent = DirectX::XMVector3Cross(upVector, tangent);

	return DirectX::XMVector3Normalize(bitangent);
}

DirectX::XMVECTOR RoadMeshGenerator::CalculateNormal(const DirectX::XMVECTOR& tangent, const DirectX::XMVECTOR& bitangent)
{
	const auto normal = DirectX::XMVector3Cross(tangent, bitangent);
	return DirectX::XMVector3Normalize(normal);
}
