#pragma once
#include <vector>
#include "../Renderer/Mesh.h"
#include <unordered_map>

class ModelLoader final
{
public:
	static void LoadResource(ID3D11Device *device, const std::string& path, const std::string& resourceId);
	static std::shared_ptr<std::vector<Mesh>> GetResource(const std::string& resourceId);
private:
	inline static std::unordered_map<std::string, std::shared_ptr<std::vector<Mesh>>> MeshesResources;
};
