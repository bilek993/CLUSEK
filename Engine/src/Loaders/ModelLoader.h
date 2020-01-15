#pragma once
#include <vector>
#include "../Renderer/Mesh.h"
#include <unordered_map>
#include <mutex>

class ModelLoader final
{
public:
	static void LoadResource(ID3D11Device *device, std::string path, std::string resourceId);
	static std::shared_ptr<std::vector<Mesh>> GetResource(const std::string& resourceId);

private:
	static inline std::mutex DeviceMutex{};
	static inline std::mutex ResourcesMapMutex{};

	static std::unordered_map<std::string, std::shared_ptr<std::vector<Mesh>>> MeshesResources;
};
