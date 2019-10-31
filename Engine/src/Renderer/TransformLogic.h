#pragma once
#include <DirectXMath.h>
#include "PxPhysicsAPI.h"
#include "../ECS/Components/TransformComponent.h"

class TransformLogic final
{
public:
	static void SetMatrix(const DirectX::XMMATRIX &matrix, TransformComponent &transformComponent);
	static void SetMatrix(const physx::PxMat44, TransformComponent &transformComponent);

	static void SetPosition(const DirectX::XMVECTOR &pos,TransformComponent &transformComponent);
	static void SetPosition(float x, float y, float z, TransformComponent &transformComponent);

	static void SetRotation(const DirectX::XMVECTOR &rot, TransformComponent &transformComponent);
	static void SetRotation(float x, float y, float z, TransformComponent &transformComponent);
	static void SetRotation(float x, float y, float z, float w, TransformComponent &transformComponent);

	static void AdjustPosition(const DirectX::XMVECTOR &pos, TransformComponent &transformComponent);
	static void AdjustPosition(float x, float y, float z, TransformComponent &transformComponent);

	static void AdjustRotation(float x, float y, float z, TransformComponent &transformComponent);
	static void AdjustRotation(float x, float y, float z, float w, TransformComponent &transformComponent);

	static void AdjustRotationEuler(const DirectX::XMVECTOR &rot, TransformComponent &transformComponent);
	static void AdjustRotationQuat(const DirectX::XMVECTOR &rot, TransformComponent &transformComponent);

	static void GetPosition(float* x, float* y, float* z, const TransformComponent &transformComponent);
	static DirectX::XMFLOAT3 GetPosition(const TransformComponent &transformComponent);

	static void GetRotation(float* x, float* y, float* z, const TransformComponent &transformComponent);
	static void GetRotation(float* x, float* y, float* z, float* w, const TransformComponent &transformComponent);

	static DirectX::XMFLOAT3 GetRotationEuler(const TransformComponent &transformComponent);
	static DirectX::XMFLOAT4 GetRotationQuat(const TransformComponent &transformComponent);

	static void LookAt(DirectX::XMFLOAT3 targetPosition, TransformComponent &transformComponent);
};
