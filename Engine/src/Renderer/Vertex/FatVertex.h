#pragma once
#include <DirectXMath.h>
#include <d3d11.h>

struct FatVertex final
{
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT2 TextureCoord;
	DirectX::XMFLOAT3 Normal;
	DirectX::XMFLOAT3 Tangent;

	inline static D3D11_INPUT_ELEMENT_DESC Layout[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	inline static int LayoutSize = ARRAYSIZE(Layout);

	FatVertex() {}
	FatVertex(const float x, const float y, const float z, const float u, const float v, const float nx, const float ny, 
		const float nz, const float tx, const float ty, const float tz, const float bx, const float by, 
		const float bz) : Position(x, y, z), TextureCoord(u, v), Normal(nx, ny, nz), Tangent(tx, ty, tz) {}
};
