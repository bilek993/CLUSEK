#pragma once
#include <wrl/client.h>
#include "../../Renderer/Vertex.h"
#include "../../Renderer/ConstantBuffer.h"
#include "../../Renderer/Mesh.h"
#include "vector"

struct RenderComponent final
{
	std::vector<Mesh> Meshes;
	DirectX::XMMATRIX ModelMatrix;
	ConstantBuffer<CB_VS_UberVertexShader> UberShaderConstantBuffer;
};
