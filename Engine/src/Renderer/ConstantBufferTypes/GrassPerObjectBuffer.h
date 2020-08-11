#pragma once
#include <DirectXMath.h>

struct GrassPerObjectBuffer final
{
	DirectX::XMMATRIX ViewProjectionMatrix;
	DirectX::XMMATRIX LightSpaceMatrix[4];
};
