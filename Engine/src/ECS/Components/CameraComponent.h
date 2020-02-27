#pragma once
#include <DirectXMath.h>
#include <array>

struct CameraComponent final
{
	DirectX::XMMATRIX ViewMatrix;
	DirectX::XMMATRIX ProjectionMatrix;
	float MaxRotationY;
	float MinRotationY;
	std::array<DirectX::XMVECTOR,6> FrustumPlanes{};
};
