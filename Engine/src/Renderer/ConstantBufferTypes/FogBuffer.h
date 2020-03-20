#pragma once
#include <DirectXMath.h>

struct FogBuffer final
{
	DirectX::XMFLOAT3 FogColor;
	float FogDensity;
	float SkyConstantValue;
};
