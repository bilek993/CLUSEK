#pragma once
#include "../../Renderer/ConstantBuffer.h"
#include "../../Renderer/Mesh.h"
#include "vector"

struct RenderComponent final
{
	std::shared_ptr<std::vector<Mesh>> Meshes;
	DirectX::XMMATRIX ModelMatrix;
	ConstantBuffer<CB_VS_UberVertexShader> UberShaderConstantBuffer;
};
