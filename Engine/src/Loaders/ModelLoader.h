#pragma once
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <unordered_map>
#include <mutex>
#include "../Renderer/Mesh.h"
#include "../Renderer/GrassMesh.h"

class ModelLoader final
{
public:
	static void LoadResource(ID3D11Device *device, const std::string& path, const std::string& resourceId);
	static void LoadGrassResource(ID3D11Device *device, const std::string& path, const std::string& resourceId);
	
	static std::shared_ptr<std::vector<Mesh>> GetResource(const std::string& resourceId);
	static std::shared_ptr<std::vector<GrassMesh>> GetGrassResource(const std::string& resourceId);

private:
	static std::vector<FatVertex> CreateFatVertices(const aiMesh* mesh);
	static std::vector<DWORD> CreateIndices(const aiMesh* mesh);

	static inline std::mutex ResourcesMapMutex{};

	static std::unordered_map<std::string, std::shared_ptr<std::vector<Mesh>>> MeshesResources;
	static std::unordered_map<std::string, std::shared_ptr<std::vector<GrassMesh>>> GrassMeshesResources;
};
