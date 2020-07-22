#pragma once
#include <DirectXMath.h>

struct GrassInstanceBuffer final
{
	DirectX::XMFLOAT3 Position;
	float Smoothness;
	DirectX::XMFLOAT3 AlbedoColor;
	float Metalness;
};
