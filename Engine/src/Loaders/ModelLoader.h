#pragma once
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <unordered_map>
#include <mutex>
#include <PxPhysics.h>
#include <cooking/PxCooking.h>
#include "../Renderer/Mesh.h"
#include "../Renderer/GrassMesh.h"

class ModelLoader final
{
public:
	static void LoadResource(ID3D11Device *device, const std::string& path, const std::string& resourceId);
	static void LoadGrassResource(ID3D11Device *device, const std::string& path, const std::string& resourceId);
	static void LoadConvexResource(physx::PxPhysics& physics, const physx::PxCooking& cooking, const std::string& path, const std::string& resourceId);
	
	static std::shared_ptr<std::vector<Mesh>> GetResource(const std::string& resourceId);
	static std::shared_ptr<std::vector<GrassMesh>> GetGrassResource(const std::string& resourceId);
	static physx::PxConvexMesh* GetConvexResource(const std::string& resourceId);

	static std::vector< physx::PxConvexMesh*> GetAllConvexResources();

private:
	static std::vector<FatVertex> CreateFatVertices(const aiMesh* mesh);
	static std::vector<DWORD> CreateIndices(const aiMesh* mesh);

	static inline std::mutex ResourcesMapMutex{};

	static std::unordered_map<std::string, std::shared_ptr<std::vector<Mesh>>> MeshesResources;
	static std::unordered_map<std::string, std::shared_ptr<std::vector<GrassMesh>>> GrassMeshesResources;
	static std::unordered_map<std::string, physx::PxConvexMesh*> ConvexMeshesResources;
};
