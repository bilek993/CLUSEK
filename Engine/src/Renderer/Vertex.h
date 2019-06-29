#pragma once
#include <DirectXMath.h>

struct Vertex final
{
	DirectX::XMFLOAT2 Position;

	Vertex() {};
	Vertex(const float x, const float y) : Position(x,y) {}
};