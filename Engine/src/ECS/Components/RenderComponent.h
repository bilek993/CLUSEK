#pragma once
#include <wrl/client.h>
#include "../../Renderer/IndexBuffer.h"
#include "../../Renderer/Vertex.h"
#include "../../Renderer/ConstantBuffer.h"

struct RenderComponent final
{
	DirectX::XMMATRIX ModelMatrix;
	VertexBuffer<Vertex> RenderVertexBuffer;
	IndexBuffer RenderIndexBuffer;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> TextureResourceView;
	ConstantBuffer<CB_VS_UberVertexShader> UberShaderConstantBuffer;
};
