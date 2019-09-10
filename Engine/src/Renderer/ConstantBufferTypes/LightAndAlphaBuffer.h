#pragma once
#include <DirectXMath.h>

struct LightAndAlphaBuffer final
{
	DirectX::XMFLOAT3 DirectionalLightColor;
	float DirectionalLightStrength;
	DirectX::XMFLOAT3 DirectionalLightDirection;
	float Alpha;
};
