#pragma once
#include <wrl/client.h>
#include "../../Renderer/IndexBuffer.h"
#include "../../Renderer/Vertex.h"

struct RenderComponent final
{
	VertexBuffer<Vertex> RenderVertexBuffer;
	IndexBuffer RenderIndexBuffer;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> TextureResourceView;
	ConstantBuffer<CB_VS_UberVertexShader> UberShaderConstantBuffer;
};
