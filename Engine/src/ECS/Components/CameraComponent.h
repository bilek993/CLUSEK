#pragma once
#include <DirectXMath.h>

struct CameraComponent final
{
	DirectX::XMMATRIX ViewMatrix;
	DirectX::XMMATRIX ProjectionMatrix;
	float MaxRotationY;
	float MinRotationY;
	DirectX::XMVECTOR FrustumPlanes[6]{};
};
