#pragma once
#include <DirectXMath.h>

struct LightSettings final
{
	DirectX::XMFLOAT3 AmbientLightColor = DirectX::XMFLOAT3(0.6f, 0.8f, 1.0f);
	float AmbientLightStrength = 0.5f;

	DirectX::XMFLOAT3 DirectionalLightColor = DirectX::XMFLOAT3(0.9f, 0.9f, 1.0f);
	float DirectionalLightStrength = 0.75f;
	DirectX::XMFLOAT3 DirectionalLightDirection = DirectX::XMFLOAT3(0.25f, 0.5f, -1.0f);
};
