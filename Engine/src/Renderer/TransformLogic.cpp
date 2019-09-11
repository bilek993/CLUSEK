#include "TransformLogic.h"
#include "../Utils/Logger.h"

void TransformLogic::SetPosition(const DirectX::XMVECTOR& pos, TransformComponent& transformComponent)
{
	transformComponent.PositionVector = pos;
}

void TransformLogic::SetPosition(const float x, const float y, const float z, TransformComponent& transformComponent)
{
	auto newPosition = DirectX::XMFLOAT3(x, y, z);
	transformComponent.PositionVector = XMLoadFloat3(&newPosition);
}

void TransformLogic::SetRotation(const DirectX::XMVECTOR& rot, TransformComponent& transformComponent)
{
	transformComponent.RotationVector = rot;
}

void TransformLogic::SetRotation(const float x, const float y, const float z, TransformComponent& transformComponent)
{
	auto newRotation = DirectX::XMFLOAT3(x, y, z);
	transformComponent.RotationVector = XMLoadFloat3(&newRotation);
}

void TransformLogic::GetPosition(float* x, float* y, float* z, const TransformComponent& transformComponent)
{
	DirectX::XMFLOAT3 storedValue;
	XMStoreFloat3(&storedValue, transformComponent.PositionVector);

	if (x != nullptr)
		*x = storedValue.x;
	if (y != nullptr)
		*y = storedValue.y;
	if (z != nullptr)
		*z = storedValue.z;
}

DirectX::XMFLOAT3 TransformLogic::GetPosition(const TransformComponent& transformComponent)
{
	DirectX::XMFLOAT3 storedValue;
	XMStoreFloat3(&storedValue, transformComponent.PositionVector);
	return storedValue;
}

void TransformLogic::AdjustPosition(const DirectX::XMVECTOR& pos, TransformComponent& transformComponent)
{
	transformComponent.PositionVector = DirectX::XMVectorAdd(transformComponent.PositionVector, pos);
}

void TransformLogic::AdjustPosition(const float x, const float y, const float z, TransformComponent& transformComponent)
{
	DirectX::XMFLOAT3 storedValue;
	XMStoreFloat3(&storedValue, transformComponent.PositionVector);
	storedValue.x += x;
	storedValue.y += y;
	storedValue.z += z;
	transformComponent.PositionVector = XMLoadFloat3(&storedValue);
}

void TransformLogic::AdjustRotation(const DirectX::XMVECTOR& rot, TransformComponent& transformComponent)
{
	transformComponent.RotationVector = DirectX::XMVectorAdd(transformComponent.RotationVector, rot);;
}

void TransformLogic::AdjustRotation(const float x, const float y, const float z, TransformComponent& transformComponent)
{
	DirectX::XMFLOAT3 storedValue;
	XMStoreFloat3(&storedValue, transformComponent.RotationVector);
	storedValue.x += x;
	storedValue.y += y;
	storedValue.z += z;
	transformComponent.RotationVector = XMLoadFloat3(&storedValue);
}

void TransformLogic::GetRotation(float* x, float* y, float* z, const TransformComponent& transformComponent)
{
	DirectX::XMFLOAT3 storedValue;
	XMStoreFloat3(&storedValue, transformComponent.RotationVector);

	if (x != nullptr)
		*x = storedValue.x;
	if (y != nullptr)
		*y = storedValue.y;
	if (z != nullptr)
		*z = storedValue.z;
}

DirectX::XMFLOAT3 TransformLogic::GetRotation(const TransformComponent& transformComponent)
{
	DirectX::XMFLOAT3 storedValue;
	XMStoreFloat3(&storedValue, transformComponent.RotationVector);
	return storedValue;
}

void TransformLogic::LookAt(DirectX::XMFLOAT3 targetPosition, TransformComponent& transformComponent)
{
	DirectX::XMFLOAT3 currentCameraPosition;
	XMStoreFloat3(&currentCameraPosition, transformComponent.PositionVector);

	if (currentCameraPosition.x == targetPosition.x && currentCameraPosition.y == targetPosition.y && currentCameraPosition.z == targetPosition.z)
	{
		Logger::Warning("Camera position cannot be the save as target position!");
		return;
	}

	targetPosition.x = currentCameraPosition.x - targetPosition.x;
	targetPosition.y = currentCameraPosition.y - targetPosition.y;
	targetPosition.z = currentCameraPosition.z - targetPosition.z;

	const auto distance = sqrt((targetPosition.x * targetPosition.x) + (targetPosition.z * targetPosition.z));
	const auto pitch = atan(targetPosition.y / distance);

	auto yaw = atan(targetPosition.x / targetPosition.z);;

	if (targetPosition.z > 0)
		yaw += DirectX::XM_PI;

	SetRotation(pitch, yaw, 0.0f, transformComponent);
}
