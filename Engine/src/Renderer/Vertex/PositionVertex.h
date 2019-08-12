#pragma once
#include <DirectXMath.h>
#include <d3d11.h>

struct PositionVertex final
{
	DirectX::XMFLOAT3 Position;

	inline static D3D11_INPUT_ELEMENT_DESC Layout[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	inline static int LayoutSize = ARRAYSIZE(Layout);

	PositionVertex() {}
	PositionVertex(const float x, const float y, const float z) : Position(x, y, z) {}
};
