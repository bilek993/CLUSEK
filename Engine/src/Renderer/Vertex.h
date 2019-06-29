#pragma once
#include <DirectXMath.h>

struct Vertex final
{
	DirectX::XMFLOAT2 Position;
	DirectX::XMFLOAT3 Color;

	Vertex() {};
	Vertex(const float x, const float y, const float r, const float g, const float b) : Position(x, y), Color(r, g, b) {}
};