#pragma once
#include <DirectXMath.h>

struct CameraComponent final
{
	DirectX::XMMATRIX ViewMatrix;
	DirectX::XMMATRIX ProjectionMatrix;

	DirectX::XMVECTOR PositionVector;
	DirectX::XMVECTOR RotationVector;

	DirectX::XMVECTOR VectorForward;
	DirectX::XMVECTOR VectorRight;

	inline static const DirectX::XMVECTOR FORWARD_VECTOR = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	inline static const DirectX::XMVECTOR UP_VECTOR = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	inline static const DirectX::XMVECTOR RIGHT_VECTOR = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
};
