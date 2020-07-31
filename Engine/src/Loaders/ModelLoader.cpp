#include "ModelLoader.h"
#include "../Renderer/FrustumUtil.h"

std::unordered_map<std::string, std::shared_ptr<std::vector<Mesh>>> ModelLoader::MeshesResources;
std::unordered_map<std::string, std::shared_ptr<std::vector<GrassMesh>>> ModelLoader::GrassMeshesResources;

void ModelLoader::LoadResource(ID3D11Device *device, const std::string& path, const std::string& resourceId)
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
		auto vertices = CreateFatVertices(mesh);
		auto indices = CreateIndices(mesh);

		Mesh newMesh;
		newMesh.Name = mesh->mName.C_Str();
		newMesh.FrustumPoints = FrustumUtil::CalculateAABB(vertices);

		auto hr = newMesh.RenderIndexBuffer.Initialize(device, indices.data(), indices.size());
		if (FAILED(hr))
			continue;
		hr = newMesh.RenderVertexBuffer.Initialize(device, vertices.data(), vertices.size());
		if (FAILED(hr))
			continue;

		loadedMeshes->emplace_back(newMesh);

		Logger::Debug("Mesh '" + newMesh.Name + "' added into the model!");
	}

	ResourcesMapMutex.lock();
	MeshesResources[resourceId] = loadedMeshes;
	ResourcesMapMutex.unlock();
}

void ModelLoader::LoadGrassResource(ID3D11Device* device, const std::string& path, const std::string& resourceId)
{
	Logger::Debug("Preparing to load grass mesh from: '" + path + "'...");

	auto loadedMeshes = std::make_shared<std::vector<GrassMesh>>();
	Assimp::Importer importer;

	Logger::Debug("Reading data from file...");
	const auto scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_ConvertToLeftHanded | aiProcess_CalcTangentSpace);
	if (scene == nullptr)
		return;

	const auto meshes = scene->mMeshes;
	for (unsigned int i = 0; i < scene->mNumMeshes; i++)
	{
		const auto mesh = meshes[i];
		auto vertices = CreateFatVertices(mesh);
		auto indices = CreateIndices(mesh);

		GrassMesh newMesh;
		newMesh.Name = mesh->mName.C_Str();

		auto hr = newMesh.RenderIndexBuffer.Initialize(device, indices.data(), indices.size());
		if (FAILED(hr))
			continue;
		hr = newMesh.RenderVertexBuffer.Initialize(device, vertices.data(), vertices.size());
		if (FAILED(hr))
			continue;

		loadedMeshes->emplace_back(newMesh);

		Logger::Debug("Grass mesh '" + newMesh.Name + "' added into the model!");
	}

	ResourcesMapMutex.lock();
	GrassMeshesResources[resourceId] = loadedMeshes;
	ResourcesMapMutex.unlock();
}

std::shared_ptr<std::vector<Mesh>> ModelLoader::GetResource(const std::string& resourceId)
{
	const auto meshPointer = MeshesResources.find(resourceId);
	if (meshPointer == MeshesResources.end())
		Logger::Error("Resource with id '" + resourceId + "' not found!");

	return meshPointer->second;
}

std::shared_ptr<std::vector<GrassMesh>> ModelLoader::GetGrassResource(const std::string& resourceId)
{
	const auto meshPointer = GrassMeshesResources.find(resourceId);
	if (meshPointer == GrassMeshesResources.end())
		Logger::Error("Resource with id '" + resourceId + "' not found!");

	return meshPointer->second;
}

std::vector<FatVertex> ModelLoader::CreateFatVertices(const aiMesh* mesh)
{
	std::vector<FatVertex> vertices;

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

	return vertices;
}

std::vector<DWORD> ModelLoader::CreateIndices(const aiMesh* mesh)
{
	std::vector<DWORD> indices;

	for (unsigned int j = 0; j < mesh->mNumFaces; j++)
	{
		const auto face = mesh->mFaces[j];

		for (unsigned int k = 0; k < face.mNumIndices; k++)
		{
			indices.emplace_back(face.mIndices[k]);
		}
	}

	return indices;
}
