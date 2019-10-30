#pragma once
#include <DirectXMath.h>
#include "../../Renderer/RotationMode.h"

struct TransformComponent final
{
	DirectX::XMVECTOR PositionVector;
	DirectX::XMVECTOR RotationVectorEuler;
	DirectX::XMVECTOR RotationVectorQuat;

	DirectX::XMVECTOR VectorForward;
	DirectX::XMVECTOR VectorRight;

	bool ValuesChanged = true;
	RotationMode RotationModeForChanges = EulerAngels;

	inline static const DirectX::XMVECTOR FORWARD_VECTOR = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	inline static const DirectX::XMVECTOR UP_VECTOR = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	inline static const DirectX::XMVECTOR RIGHT_VECTOR = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
};
