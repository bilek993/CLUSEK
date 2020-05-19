#pragma once
#include "../../Renderer/Mesh.h"
#include "vector"

struct ModelRenderComponent final
{
	std::string ModelId;
	std::string LowPolyModelId;
	std::string MaterialId;
	std::shared_ptr<std::vector<Mesh>> Meshes;
	std::shared_ptr<std::vector<Mesh>> LowPolyMeshes;
};
