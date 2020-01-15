#include "ModelLoader.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

std::unordered_map<std::string, std::shared_ptr<std::vector<Mesh>>> ModelLoader::MeshesResources;

void ModelLoader::LoadResource(ID3D11Device *device, const std::string path, const std::string resourceId)
{
	Logger::Debug("Preparing to load mesh from: '" + path + "'...");

	auto loadedMeshes = std::make_shared<std::vector<Mesh>>();
	Assimp::Importer importer;

	Logger::Debug("Reading data from file...");
	const auto scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_ConvertToLeftHanded | aiProcess_CalcTangentSpace);
	if (scene == nullptr)
		return;

	const auto meshes = scene->mMeshes;
	for (unsigned int i = 0; i < scene->mNumMeshes; i++)
	{
		auto mesh = meshes[i];
		std::vector<FatVertex> vertices;
		std::vector<DWORD> indices;

		for (unsigned int j = 0; j < mesh->mNumVertices; j++)
		{
			FatVertex vertex;

			vertex.Position.x = mesh->mVertices[j].x;
			vertex.Position.y = mesh->mVertices[j].y;
			vertex.Position.z = mesh->mVertices[j].z;

			vertex.Normal.x = mesh->mNormals[j].x;
			vertex.Normal.y = mesh->mNormals[j].y;
			vertex.Normal.z = mesh->mNormals[j].z;

			vertex.Tangent.x = mesh->mTangents[j].x;
			vertex.Tangent.y = mesh->mTangents[j].y;
			vertex.Tangent.z = mesh->mTangents[j].z;

			if (mesh->mTextureCoords[0])
			{
				vertex.TextureCoord.x = static_cast<float>(mesh->mTextureCoords[0][j].x);
				vertex.TextureCoord.y = static_cast<float>(mesh->mTextureCoords[0][j].y);
			}

			vertices.emplace_back(vertex);
		}

		for (unsigned int j = 0; j < mesh->mNumFaces; j++)
		{
			const auto face = mesh->mFaces[j];

			for (unsigned int k = 0; k < face.mNumIndices; k++)
			{
				indices.emplace_back(face.mIndices[k]);
			}
		}

		Mesh newMesh;
		newMesh.Name = mesh->mName.C_Str();

		DeviceMutex.lock();
		auto hr = newMesh.RenderIndexBuffer.Initialize(device, indices.data(), indices.size());
		if (FAILED(hr))
			continue;
		hr = newMesh.RenderVertexBuffer.Initialize(device, vertices.data(), vertices.size());
		if (FAILED(hr))
			continue;
		DeviceMutex.unlock();

		loadedMeshes->emplace_back(newMesh);

		Logger::Debug("Mesh '" + newMesh.Name + "' added into the model!");
	}

	ResourcesMapMutex.lock();
	MeshesResources[resourceId] = loadedMeshes;
	ResourcesMapMutex.unlock();
}

std::shared_ptr<std::vector<Mesh>> ModelLoader::GetResource(const std::string& resourceId)
{
	const auto meshPointer = MeshesResources.find(resourceId);
	if (meshPointer == MeshesResources.end())
		Logger::Error("Resource with id '" + resourceId + "' not found!");

	return meshPointer->second;
}
