#pragma once
#include "../../Renderer/ConstantBuffer.h"
#include "../../Renderer/Mesh.h"
#include "vector"

struct RenderComponent final
{
	std::string ModelId;
	std::string MaterialId;
	std::shared_ptr<std::vector<Mesh>> Meshes;
	DirectX::XMMATRIX ModelMatrix;
};
