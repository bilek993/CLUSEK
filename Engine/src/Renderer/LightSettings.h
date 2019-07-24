#pragma once
#include <DirectXMath.h>

struct LightSettings final
{
	DirectX::XMFLOAT3 AmbientLightColor = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	float AmbientLightStrength = 1.0f;
};
