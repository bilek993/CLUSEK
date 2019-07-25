#pragma once
#include <DirectXMath.h>

struct Vertex final
{
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT2 TextureCoord;
	DirectX::XMFLOAT3 Normal;

	Vertex() {}
	Vertex(const float x, const float y, const float z, const float u, const float v, const float nx, const float ny,
	       const float nz) : Position(x, y, z), TextureCoord(u, v), Normal(nx, ny, nz) {}
};