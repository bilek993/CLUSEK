#include "RoadMeshGenerator.h"
#include "../Renderer/FrustumUtil.h"

void RoadMeshGenerator::GenerateRoadMesh(ID3D11Device* device, RoadComponent& roadComponent)
{
	GenerateVertices(device, roadComponent);
	GenerateIndices(device, roadComponent);
}

void RoadMeshGenerator::GenerateVertices(ID3D11Device* device, RoadComponent& roadComponent)
{
	Logger::Debug("Generating road vertices...");

	auto distanceSinceStart = 0.0f;
	
	const auto vertexCount = roadComponent.MeshVertices.size() * roadComponent.CalculatedSupportPoints.size();
	std::vector<FatVertex> vertices{};

	for (auto i = 0; i < roadComponent.CalculatedSupportPoints.size(); i++)
	{
		const auto previousPoint = (i - 1) < 0 ? nullptr : &roadComponent.CalculatedSupportPoints[i - 1];
		const auto currentPoint = &roadComponent.CalculatedSupportPoints[i];
		const auto nextPoint = (i + 1) >= roadComponent.CalculatedSupportPoints.size() ? nullptr : &roadComponent.CalculatedSupportPoints[i + 1];

		const auto tangentVector = CalculateTangent(previousPoint, currentPoint, nextPoint);
		const auto bitangentVector = CalculateBitangent(tangentVector);
		const auto normalVector = CalculateNormal(tangentVector, bitangentVector);
		
		for (auto j = 0; j < roadComponent.MeshVertices.size(); j++)
		{
			const auto positionVector = CalculatePosition(	tangentVector,
															bitangentVector,
															normalVector,
															roadComponent.MeshVertices[j],
															*currentPoint);
			
			FatVertex vertex;

			XMStoreFloat3(&vertex.Position, positionVector);
			XMStoreFloat3(&vertex.Normal, normalVector);
			XMStoreFloat3(&vertex.Tangent, tangentVector);
			vertex.TextureCoord = CalculateTextureCoord(j, 
														roadComponent.MeshVertices.size(),
														distanceSinceStart,
														roadComponent.TextureScaling,
														previousPoint,
														currentPoint);
			
			vertices.emplace_back(vertex);
		}
	}

	roadComponent.Mesh.FrustumPoints = FrustumUtil::CalculateAABB(vertices);
	roadComponent.Mesh.RenderVertexBuffer.Initialize(device, vertices.data(), vertexCount);
}

void RoadMeshGenerator::GenerateIndices(ID3D11Device* device, RoadComponent& roadComponent)
{
	Logger::Debug("Generating road indices...");
	
	const auto indicesCount = (roadComponent.CalculatedSupportPoints.size() - 1) * (roadComponent.MeshVertices.size() - 1) * 6;
	std::vector<DWORD> indices{};

	for (auto i = 0; i < roadComponent.CalculatedSupportPoints.size() - 1; i++)
	{
		for (auto j = 0; j < roadComponent.MeshVertices.size() - 1; j++)
		{
			// First triangle
			indices.emplace_back((roadComponent.MeshVertices.size() * (i + 1)) + j);
			indices.emplace_back((roadComponent.MeshVertices.size() * (i + 1)) + (j + 1));
			indices.emplace_back((roadComponent.MeshVertices.size() * i) + j);

			// Second triangle
			indices.emplace_back((roadComponent.MeshVertices.size() * (i + 1)) + (j + 1));
			indices.emplace_back((roadComponent.MeshVertices.size() * i) + (j + 1));
			indices.emplace_back((roadComponent.MeshVertices.size() * i) + j);
		}
	}

	roadComponent.Mesh.RenderIndexBuffer.Initialize(device, indices.data(), indicesCount);
}

DirectX::XMVECTOR RoadMeshGenerator::CalculateTangent(const DirectX::XMVECTOR* previousPoint,
	const DirectX::XMVECTOR* currentPoint, const DirectX::XMVECTOR* nextPoint)
{
	auto directionVector = DirectX::XMVectorZero();

	if (previousPoint != nullptr)
		directionVector = DirectX::XMVectorAdd(directionVector, DirectX::XMVectorSubtract(*currentPoint, *previousPoint));
	
	if (nextPoint != nullptr)
		directionVector = DirectX::XMVectorAdd(directionVector, DirectX::XMVectorSubtract(*nextPoint, *currentPoint));
	
	return DirectX::XMVector3Normalize(directionVector);
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

DirectX::XMVECTOR RoadMeshGenerator::CalculatePosition(const DirectX::XMVECTOR& tangent, const DirectX::XMVECTOR& bitangent, 
	const DirectX::XMVECTOR& normal, const DirectX::XMFLOAT2 meshVertex, const DirectX::XMVECTOR& currentPoint)
{
	const DirectX::XMFLOAT3 meshVertexFloats3(meshVertex.x, meshVertex.y, 0.0f);
	const auto meshVertexVector = XMLoadFloat3(&meshVertexFloats3);
	
	DirectX::XMFLOAT3 tangentFloats{};
	DirectX::XMFLOAT3 bitangentFloats{};
	DirectX::XMFLOAT3 normalFloats{};

	XMStoreFloat3(&tangentFloats, tangent);
	XMStoreFloat3(&bitangentFloats, bitangent);
	XMStoreFloat3(&normalFloats, normal);

	DirectX::XMFLOAT3X3 bitangentTangentNormalFloats(	bitangentFloats.x, bitangentFloats.y, bitangentFloats.z,
														tangentFloats.x, tangentFloats.y, tangentFloats.z,
														normalFloats.x, normalFloats.y, normalFloats.z);

	const auto bitangentTangentNormalMatrix = XMLoadFloat3x3(&bitangentTangentNormalFloats);

	return DirectX::XMVectorAdd(XMVector3Transform(meshVertexVector, bitangentTangentNormalMatrix), currentPoint);
}

DirectX::XMFLOAT2 RoadMeshGenerator::CalculateTextureCoord(const int vertexId, const int vertexCount, float& distanceSinceStart,
	const float textureScaling, const DirectX::XMVECTOR* previousPoint, const DirectX::XMVECTOR* currentPoint)
{
	const auto u = static_cast<float>(vertexId) / static_cast<float>(vertexCount - 1);
	auto v = 0.0f;

	if (previousPoint != nullptr && currentPoint != nullptr)
	{
		const auto distance = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(*currentPoint, *previousPoint)));
		distanceSinceStart += distance;
		v += distanceSinceStart / textureScaling;
	}
	
	return DirectX::XMFLOAT2(u, v);
}
