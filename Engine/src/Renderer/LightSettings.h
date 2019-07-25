#pragma once
#include <DirectXMath.h>

struct LightSettings final
{
	DirectX::XMFLOAT3 AmbientLightColor = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	float AmbientLightStrength = 1.0f;

	DirectX::XMFLOAT3 DirectionalLightColor = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	float DirectionalLightStrength = 1.0f;
	DirectX::XMFLOAT3 DirectionalLightDirection = DirectX::XMFLOAT3(0.25f, 0.5f, -1.0f);
};
