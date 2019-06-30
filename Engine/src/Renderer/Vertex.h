#pragma once
#include <DirectXMath.h>

struct Vertex final
{
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT3 Color;

	Vertex() {};
	Vertex(const float x, const float y, const float z, const float r, const float g, const float b) : Position(x, y, z), Color(r, g, b) {}
};