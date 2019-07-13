#pragma once
#include <DirectXMath.h>

struct CameraComponent final
{
	DirectX::XMMATRIX ViewMatrix;
	DirectX::XMMATRIX ProjectionMatrix;
};
