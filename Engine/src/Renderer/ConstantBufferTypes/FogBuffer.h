#pragma once
#include <DirectXMath.h>

struct FogBuffer final
{
	DirectX::XMFLOAT3 FogColor;
	float FogDensity;
	DirectX::XMFLOAT3 FogLightColor;
	float FogLightPower;
	float FogMinDistance;
	float SkyConstantValue;
};
