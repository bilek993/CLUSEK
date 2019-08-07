#pragma once
#include "../../Renderer/Mesh.h"
#include "vector"

struct SkyboxComponent final
{
	std::string ModelId;
	std::string SkyboxTextureId;
	std::shared_ptr<std::vector<Mesh>> Meshes;
	DirectX::XMMATRIX WorldMatrix;
};