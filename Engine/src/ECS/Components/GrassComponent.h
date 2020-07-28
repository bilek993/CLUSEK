#pragma once
#include <vector>
#include "../../Renderer/Mesh.h"

struct GrassComponent final
{
	std::string ModelId;
	std::string MaterialId;

	std::shared_ptr<std::vector<Mesh>> Meshes;
};
