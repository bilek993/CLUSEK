#include "RoadMeshGenerator.h"

#include <future>

#include "SplineUtil.h"
#include "../Loaders/MaterialLoader.h"
#include "../Renderer/FrustumUtil.h"

void RoadMeshGenerator::RegenerateRoadComponent(ID3D11Device* device, RoadComponent& roadComponent, const bool async)
{
	Logger::Debug("Regenerating road mesh...");

	ClearOldComponentData(roadComponent);
	GenerateSupportPoints(roadComponent, async);
	GenerateRoadMesh(device, roadComponent, async);
	
	MaterialLoader::SetResourceForSingleMesh(device, roadComponent.Mesh, roadComponent.MaterialId);

	Logger::Debug("Reinitialized road mesh successfully!");
	roadComponent.InitializedOnce = true;
}

void RoadMeshGenerator::ClearOldComponentData(RoadComponent& roadComponent)
{
	if (roadComponent.InitializedOnce)
	{
		Logger::Debug("Clearing old road data...");
		roadComponent.CalculatedSupportPoints.clear();
	}
}

void RoadMeshGenerator::GenerateSupportPoints(RoadComponent& roadComponent, const bool async)
{
	std::vector<std::future<std::vector<DirectX::XMVECTOR>>> asyncFutures{};
	
	for (auto i = 0; (i + 3) < roadComponent.Points.size(); i += 3)
	{
		if (async)
		{
			asyncFutures.emplace_back(std::async(std::launch::async, CalculatePartialLookUpTableAndInsertThem, &roadComponent, i));
		}
		else
		{
			const auto resultPartialVector = CalculatePartialLookUpTableAndInsertThem(&roadComponent, i);
			roadComponent.CalculatedSupportPoints.insert(roadComponent.CalculatedSupportPoints.cend(), resultPartialVector.cbegin(), resultPartialVector.cend());
		}
	}

	if (async)
	{
		for (auto& currentFuture : asyncFutures)
		{
			const auto futureResult = currentFuture.get();
			roadComponent.CalculatedSupportPoints.insert(roadComponent.CalculatedSupportPoints.cend(), futureResult.cbegin(), futureResult.cend());
		}
	}
}

std::vector<DirectX::XMVECTOR> RoadMeshGenerator::CalculatePartialLookUpTableAndInsertThem(const RoadComponent* roadComponent, const int firstPointId)
{
	const auto generatorFunction = [&roadComponent, firstPointId](const float t)
	{
		const auto result = SplineUtil::CalculateBezierCubicCurve(	roadComponent->Points[firstPointId],
																				roadComponent->Points[firstPointId + 1],
																				roadComponent->Points[firstPointId + 2],
																				roadComponent->Points[firstPointId + 3],
																				t);

		return XMLoadFloat3(&result);
	};

	return SplineUtil::CalculateEvenlySpaceLookUpTable(	roadComponent->Resolution,
														roadComponent->SplitDistance,
														generatorFunction);
}

void RoadMeshGenerator::GenerateRoadMesh(ID3D11Device* device, RoadComponent& roadComponent, const bool async)
{
	if (async)
	{
		const auto futureVertices = std::async(std::launch::async, GenerateVertices, device, &roadComponent);
		const auto futureIndices = std::async(std::launch::async, GenerateIndices, device, &roadComponent);
	}
	else
	{
		GenerateVertices(device, &roadComponent);
		GenerateIndices(device, &roadComponent);
	}
}

void RoadMeshGenerator::GenerateVertices(ID3D11Device* device, RoadComponent* roadComponent)
{
	Logger::Debug("Generating road vertices...");

	auto distanceSinceStart = 0.0f;

	const auto vertexCount = roadComponent->MeshVertices.size() * roadComponent->CalculatedSupportPoints.size();
	std::vector<FatVertex> vertices{};

	for (auto i = 0; i < roadComponent->CalculatedSupportPoints.size(); i++)
	{
		const auto previousPoint = (i - 1) < 0 ? nullptr : &roadComponent->CalculatedSupportPoints[i - 1];
		const auto currentPoint = &roadComponent->CalculatedSupportPoints[i];
		const auto nextPoint = (i + 1) >= roadComponent->CalculatedSupportPoints.size() ? nullptr : &roadComponent->CalculatedSupportPoints[i + 1];

		const auto tangentVector = CalculateTangent(previousPoint, currentPoint, nextPoint);
		const auto bitangentVector = CalculateBitangent(tangentVector);
		const auto normalVector = CalculateNormal(tangentVector, bitangentVector);

		const auto totalSegmentWidth = CalculateSegmentWidth(roadComponent->MeshVertices);
		
		for (auto j = 0; j < roadComponent->MeshVertices.size(); j++)
		{
			const auto positionVector = CalculatePosition(	tangentVector,
																		bitangentVector,
																		normalVector,
																		roadComponent->MeshVertices[j],
																		*currentPoint);
			
			FatVertex vertex;

			XMStoreFloat3(&vertex.Position, positionVector);
			XMStoreFloat3(&vertex.Normal, normalVector);
			XMStoreFloat3(&vertex.Tangent, tangentVector);
			vertex.TextureCoord = CalculateTextureCoord(j, 
														totalSegmentWidth,
														distanceSinceStart,
														roadComponent->TextureScaling,
														previousPoint,
														currentPoint,
														roadComponent->MeshVertices);
			
			vertices.emplace_back(vertex);
		}
	}

	roadComponent->Mesh.FrustumPoints = FrustumUtil::CalculateAABB(vertices);
	roadComponent->Mesh.RenderVertexBuffer.Initialize(device, vertices.data(), vertexCount);
}

void RoadMeshGenerator::GenerateIndices(ID3D11Device* device, RoadComponent* roadComponent)
{
	Logger::Debug("Generating road indices...");
	
	const auto indicesCount = (roadComponent->CalculatedSupportPoints.size() - 1) * (roadComponent->MeshVertices.size() - 1) * 6;
	std::vector<DWORD> indices{};

	for (auto i = 0; i < roadComponent->CalculatedSupportPoints.size() - 1; i++)
	{
		for (auto j = 0; j < roadComponent->MeshVertices.size() - 1; j++)
		{
			// First triangle
			indices.emplace_back((roadComponent->MeshVertices.size() * (i + 1)) + j);
			indices.emplace_back((roadComponent->MeshVertices.size() * (i + 1)) + (j + 1));
			indices.emplace_back((roadComponent->MeshVertices.size() * i) + j);

			// Second triangle
			indices.emplace_back((roadComponent->MeshVertices.size() * (i + 1)) + (j + 1));
			indices.emplace_back((roadComponent->MeshVertices.size() * i) + (j + 1));
			indices.emplace_back((roadComponent->MeshVertices.size() * i) + j);
		}
	}

	roadComponent->Mesh.RenderIndexBuffer.Initialize(device, indices.data(), indicesCount);
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
	const DirectX::XMFLOAT3 meshVertexFloats3(meshVertex.x, 0.0f, meshVertex.y);
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

DirectX::XMFLOAT2 RoadMeshGenerator::CalculateTextureCoord(const int vertexId, const float totalSegmentWidth, float& distanceSinceStart,
	const float textureScaling, const DirectX::XMVECTOR* previousPoint, const DirectX::XMVECTOR* currentPoint,
	const std::vector<DirectX::XMFLOAT2>& meshVertices)
{
	const auto u = CalculateSegmentWidth(meshVertices, vertexId) / totalSegmentWidth;
	auto v = 0.0f;

	if (previousPoint != nullptr && currentPoint != nullptr)
	{
		if (vertexId == 0)
		{
			const auto distance = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(*currentPoint, *previousPoint)));
			distanceSinceStart += distance;
		}
		
		v = distanceSinceStart / textureScaling;
	}
	
	return DirectX::XMFLOAT2(u, v);
}

float RoadMeshGenerator::CalculateSegmentWidth(const std::vector<DirectX::XMFLOAT2>& meshVertices, const int lastElement)
{
	const auto limit = lastElement == -1 ? meshVertices.size() : lastElement + 1;
	auto totalDistance = 0.0f;

	for (auto i = 1; i < limit; i++)
	{
		const auto& previousPoint = meshVertices[i - 1];
		const auto& currentPoint = meshVertices[i];

		totalDistance += std::sqrtf(std::powf(previousPoint.x - currentPoint.x, 2.0f) + std::powf(previousPoint.y - currentPoint.y, 2.0f));
	}
	
	return totalDistance;
}
