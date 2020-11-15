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
			FatVertex vertex;

			//vertex.Position.x = mo; TODO: Add missing implementation
			//vertex.Position.y = mesh->mVertices[j].y; TODO: Add missing implementation
			//vertex.Position.z = mesh->mVertices[j].z; TODO: Add missing implementation

			//vertex.Normal.x = mesh->mNormals[j].x; TODO: Add missing implementation
			//vertex.Normal.y = mesh->mNormals[j].y; TODO: Add missing implementation
			//vertex.Normal.z = mesh->mNormals[j].z; TODO: Add missing implementation

			//vertex.Tangent.x = mesh->mTangents[j].x; TODO: Add missing implementation
			//vertex.Tangent.y = mesh->mTangents[j].y; TODO: Add missing implementation
			//vertex.Tangent.z = mesh->mTangents[j].z; TODO: Add missing implementation

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
