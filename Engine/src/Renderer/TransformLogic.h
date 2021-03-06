#pragma once
#include <DirectXMath.h>
#include "PxPhysicsAPI.h"
#include "../ECS/Components/TransformComponent.h"

class TransformLogic final
{
public:
	static void SetMatrix(const physx::PxMat44 &matrix, TransformComponent &transformComponent);

	static void SetPositionAndRotationEuler(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& rot, TransformComponent& transformComponent);

	static void SetPosition(const DirectX::XMVECTOR &pos,TransformComponent &transformComponent);
	static void SetPosition(const DirectX::XMFLOAT3 &pos, TransformComponent &transformComponent);
	static void SetPosition(float x, float y, float z, TransformComponent &transformComponent);

	static void SetRotationEuler(const DirectX::XMVECTOR &rot, TransformComponent &transformComponent);
	static void SetRotationEuler(float x, float y, float z, TransformComponent &transformComponent);

	static void AdjustPosition(const DirectX::XMVECTOR &pos, TransformComponent &transformComponent);
	static void AdjustPosition(float x, float y, float z, TransformComponent &transformComponent);

	static void AdjustRotation(float x, float y, float z, TransformComponent &transformComponent);

	static void AdjustRotationEuler(const DirectX::XMVECTOR &rot, TransformComponent &transformComponent);

	static void GetPosition(float* x, float* y, float* z, const TransformComponent &transformComponent);
	static DirectX::XMFLOAT3 GetPosition(const TransformComponent &transformComponent);

	static void GetRotation(float* x, float* y, float* z, const TransformComponent &transformComponent);
	static void GetRotation(float* x, float* y, float* z, float* w, const TransformComponent &transformComponent);

	static DirectX::XMFLOAT3 GetRotationEuler(const TransformComponent &transformComponent);
	static DirectX::XMFLOAT4 GetRotationQuat(const TransformComponent &transformComponent);
};
