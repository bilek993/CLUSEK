#include "ModelLoader.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

std::vector<Mesh> ModelLoader::LoadMeshes(const std::string& path, ID3D11Device *device)
{
	std::vector<Mesh> returnedValue;
	Assimp::Importer importer;

	const auto scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_ConvertToLeftHanded);
	if (scene == nullptr)
		return returnedValue;

	const auto meshes = scene->mMeshes;
	for (auto i = 0; i < scene->mNumMeshes; i++)
	{
		auto mesh = meshes[i];
		std::vector<Vertex> vertices;
		std::vector<DWORD> indices;

		for (auto j = 0; j < mesh->mNumVertices; j++)
		{
			Vertex vertex;

			vertex.Position.x = mesh->mVertices[j].x;
			vertex.Position.y = mesh->mVertices[j].y;
			vertex.Position.z = mesh->mVertices[j].z;

			if (mesh->mTextureCoords[0])
			{
				vertex.TextureCoord.x = static_cast<float>(mesh->mTextureCoords[0][j].x);
				vertex.TextureCoord.y = static_cast<float>(mesh->mTextureCoords[0][j].y);
			}

			vertices.emplace_back(vertex);
		}

		for (auto j = 0; j < mesh->mNumFaces; j++)
		{
			const auto face = mesh->mFaces[j];

			for (auto k = 0; k < face.mNumIndices; k++)
			{
				indices.emplace_back(face.mIndices[k]);
			}
		}

		Mesh meshForAdd;
		meshForAdd.Name = mesh->mName.C_Str();
		auto hr = meshForAdd.RenderIndexBuffer.Initialize(device, indices.data(), indices.size());
		if (FAILED(hr))
			continue;
		hr = meshForAdd.RenderVertexBuffer.Initialize(device, vertices.data(), vertices.size());
		if (FAILED(hr))
			continue;
		returnedValue.emplace_back(meshForAdd);
	}

	return returnedValue;
}
