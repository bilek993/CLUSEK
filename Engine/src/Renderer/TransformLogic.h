#pragma once
#include <DirectXMath.h>
#include "../ECS/Components/TransformComponent.h"

class TransformLogic final
{
public:
	static void SetPosition(const DirectX::XMVECTOR &pos,TransformComponent &transformComponent);
	static void SetPosition(float x, float y, float z, TransformComponent &transformComponent);
	static void SetRotation(const DirectX::XMVECTOR &rot, TransformComponent &transformComponent);
	static void SetRotation(float x, float y, float z, TransformComponent &transformComponent);

	static void GetPositionXyz(float* x, float* y, float* z, const TransformComponent &transformComponent);

	static void AdjustPosition(const DirectX::XMVECTOR &pos, TransformComponent &transformComponent);
	static void AdjustPosition(float x, float y, float z, TransformComponent &transformComponent);
	static void AdjustRotation(const DirectX::XMVECTOR &rot, TransformComponent &transformComponent);
	static void AdjustRotation(float x, float y, float z, TransformComponent &transformComponent);

	static void GetRotationXyz(float* x, float* y, float* z, const TransformComponent &transformComponent);

	static void LookAt(DirectX::XMFLOAT3 targetPosition, TransformComponent &transformComponent);
};
