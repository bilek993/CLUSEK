#pragma once
#include <vector>
#include "../Renderer/Mesh.h"

class ModelLoader final
{
public:
	static std::vector<Mesh> LoadMeshes(const std::string& path, ID3D11Device *device);
};
