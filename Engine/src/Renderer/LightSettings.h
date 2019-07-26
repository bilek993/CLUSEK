#pragma once
#include <DirectXMath.h>

struct LightSettings final
{
	DirectX::XMFLOAT3 AmbientLightColor;
	float AmbientLightStrength;

	DirectX::XMFLOAT3 DirectionalLightColor;
	float DirectionalLightStrength;
	DirectX::XMFLOAT3 DirectionalLightDirection;
};
