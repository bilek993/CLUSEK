#pragma once
#include <DirectXMath.h>

struct TransformComponent final
{
	DirectX::XMFLOAT3 InitialPositionVector;
	DirectX::XMFLOAT3 InitialRotationVector;

	DirectX::XMMATRIX WorldMatrix;

	inline static const DirectX::XMVECTOR FORWARD_VECTOR = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	inline static const DirectX::XMVECTOR UP_VECTOR = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	inline static const DirectX::XMVECTOR RIGHT_VECTOR = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
};
