#pragma once
#include <DirectXMath.h>

struct Vertex final
{
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT2 TextureCoord;

	Vertex() {};
	Vertex(const float x, const float y, const float z, const float u, const float v) : Position(x, y, z), TextureCoord(u, v) {}
};