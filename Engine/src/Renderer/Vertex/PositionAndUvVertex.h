#pragma once
#include <DirectXMath.h>
#include <d3d11.h>

struct PositionAndUvVertex final
{
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT2 TextureCoord;

	inline static D3D11_INPUT_ELEMENT_DESC Layout[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	inline static int LayoutSize = ARRAYSIZE(Layout);

	PositionAndUvVertex() {}
	PositionAndUvVertex(const float x, const float y, const float z, const float u, const float v) : Position(x, y, z), TextureCoord(u, v) {}
};
