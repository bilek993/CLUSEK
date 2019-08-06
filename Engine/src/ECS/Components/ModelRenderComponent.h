#pragma once
#include "../../Renderer/Mesh.h"
#include "vector"

struct ModelRenderComponent final
{
	std::string ModelId;
	std::string MaterialId;
	std::shared_ptr<std::vector<Mesh>> Meshes;
	DirectX::XMMATRIX WorldMatrix;
};
