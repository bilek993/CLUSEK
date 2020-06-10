#pragma once
#include "../../Renderer/Mesh.h"
#include "vector"

struct ModelRenderComponent final
{
	bool Invisible = false;
	float LowPolyDistance = -1.0f;
	float LodTransitionDistance = 25.0f;
	std::string ModelId;
	std::string LowPolyModelId;
	std::string MaterialId;

	std::shared_ptr<std::vector<Mesh>> Meshes;
	std::shared_ptr<std::vector<Mesh>> LowPolyMeshes;
};
