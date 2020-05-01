#pragma once
#include <DirectXMath.h>
#include <array>

struct CameraComponent final
{
	DirectX::XMMATRIX ViewMatrix;
	DirectX::XMMATRIX ProjectionMatrix;
	DirectX::XMVECTOR VectorForward;
	DirectX::XMVECTOR VectorRight;
	float MaxRotationY;
	float MinRotationY;
	std::array<DirectX::XMVECTOR,6> FrustumPlanes{};
};
