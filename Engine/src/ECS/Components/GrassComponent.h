#pragma once
#include <vector>
#include "../../Renderer/GrassMesh.h"

struct GrassComponent final
{
	std::string ModelId;
	std::string MaterialId;

	std::shared_ptr<std::vector<GrassMesh>> Meshes;
};
