#pragma once
#include <DirectXMath.h>
#include <array>

struct CameraComponent final
{
	bool UseTarget = true;

	DirectX::XMMATRIX ViewMatrix;
	DirectX::XMMATRIX ProjectionMatrix;
	DirectX::XMVECTOR VectorForward;
	DirectX::XMVECTOR VectorRight;
	float MaxRotationY;
	float MinRotationY;
	std::array<DirectX::XMVECTOR,6> FrustumPlanes{};
};
