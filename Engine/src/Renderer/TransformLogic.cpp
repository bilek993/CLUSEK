#include "TransformLogic.h"
#include "../Utils/Logger.h"

void TransformLogic::SetMatrix(const physx::PxMat44& matrix, TransformComponent& transformComponent)
{
	transformComponent.ValuesChanged = true;
	transformComponent.RotationModeForChanges = ForcedMatrix;

	transformComponent.WorldMatrixForced = DirectX::XMMATRIX(
		matrix[0][0], matrix[1][0], matrix[2][0], matrix[3][0],
		matrix[0][1], matrix[1][1], matrix[2][1], matrix[3][1],
		matrix[0][2], matrix[1][2], matrix[2][2], matrix[3][2],
		matrix[0][3], matrix[1][3], matrix[2][3], matrix[3][3]
	);
}

void TransformLogic::SetPosition(const DirectX::XMVECTOR& pos, TransformComponent& transformComponent)
{
	transformComponent.ValuesChanged = true;
	transformComponent.PositionVector = pos;
}

void TransformLogic::SetPosition(const float x, const float y, const float z, TransformComponent& transformComponent)
{
	transformComponent.ValuesChanged = true;

	auto newPosition = DirectX::XMFLOAT3(x, y, z);
	transformComponent.PositionVector = XMLoadFloat3(&newPosition);
}

void TransformLogic::SetRotation(const DirectX::XMVECTOR& rot, TransformComponent& transformComponent)
{
	transformComponent.ValuesChanged = true;
	transformComponent.RotationModeForChanges = EulerAngels;

	transformComponent.RotationVectorEuler = rot;
}

void TransformLogic::SetRotation(const float x, const float y, const float z, TransformComponent& transformComponent)
{
	transformComponent.ValuesChanged = true;
	transformComponent.RotationModeForChanges = EulerAngels;

	auto newRotation = DirectX::XMFLOAT3(x, y, z);
	transformComponent.RotationVectorEuler = XMLoadFloat3(&newRotation);
}

void TransformLogic::SetRotation(const float x, const float y, const float z, const float w, TransformComponent& transformComponent)
{
	transformComponent.ValuesChanged = true;
	transformComponent.RotationModeForChanges = Quaternions;

	auto newRotation = DirectX::XMFLOAT4(x, y, z, w);
	transformComponent.RotationVectorQuat = XMLoadFloat4(&newRotation);
}

void TransformLogic::GetPosition(float* x, float* y, float* z, const TransformComponent& transformComponent)
{
	DirectX::XMFLOAT3 storedValue{};
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
	DirectX::XMFLOAT3 storedValue{};
	XMStoreFloat3(&storedValue, transformComponent.PositionVector);
	return storedValue;
}

void TransformLogic::AdjustPosition(const DirectX::XMVECTOR& pos, TransformComponent& transformComponent)
{
	transformComponent.ValuesChanged = true;

	transformComponent.PositionVector = DirectX::XMVectorAdd(transformComponent.PositionVector, pos);
}

void TransformLogic::AdjustPosition(const float x, const float y, const float z, TransformComponent& transformComponent)
{
	transformComponent.ValuesChanged = true;

	DirectX::XMFLOAT3 storedValue{};
	XMStoreFloat3(&storedValue, transformComponent.PositionVector);
	storedValue.x += x;
	storedValue.y += y;
	storedValue.z += z;
	transformComponent.PositionVector = XMLoadFloat3(&storedValue);
}

void TransformLogic::AdjustRotation(const float x, const float y, const float z, TransformComponent& transformComponent)
{
	if (transformComponent.RotationModeForChanges != EulerAngels)
		Logger::Warning("Adjusted euler angels in incorrect mode!");

	transformComponent.ValuesChanged = true;
	transformComponent.RotationModeForChanges = EulerAngels;

	DirectX::XMFLOAT3 storedValue{};
	XMStoreFloat3(&storedValue, transformComponent.RotationVectorEuler);
	storedValue.x += x;
	storedValue.y += y;
	storedValue.z += z;
	transformComponent.RotationVectorEuler = XMLoadFloat3(&storedValue);
}

void TransformLogic::AdjustRotation(const float x, const float y, const float z, const float w, TransformComponent& transformComponent)
{
	if (transformComponent.RotationModeForChanges != Quaternions)
		Logger::Warning("Adjusted quaternions in incorrect mode!");

	transformComponent.ValuesChanged = true;
	transformComponent.RotationModeForChanges = Quaternions;

	DirectX::XMFLOAT4 storedValue{};
	XMStoreFloat4(&storedValue, transformComponent.RotationVectorEuler);
	storedValue.x += x;
	storedValue.y += y;
	storedValue.z += z;
	storedValue.w += w;
	transformComponent.RotationVectorQuat = XMLoadFloat4(&storedValue);
}

void TransformLogic::AdjustRotationEuler(const DirectX::XMVECTOR& rot, TransformComponent& transformComponent)
{
	if (transformComponent.RotationModeForChanges != EulerAngels)
		Logger::Warning("Adjusted euler angels in incorrect mode!");

	transformComponent.ValuesChanged = true;
	transformComponent.RotationModeForChanges = EulerAngels;

	transformComponent.RotationVectorEuler = DirectX::XMVectorAdd(transformComponent.RotationVectorEuler, rot);
}

void TransformLogic::AdjustRotationQuat(const DirectX::XMVECTOR& rot, TransformComponent& transformComponent)
{
	if (transformComponent.RotationModeForChanges != Quaternions)
		Logger::Warning("Adjusted quaternions in incorrect mode!");

	transformComponent.ValuesChanged = true;
	transformComponent.RotationModeForChanges = Quaternions;

	transformComponent.RotationVectorQuat = DirectX::XMVectorAdd(transformComponent.RotationVectorQuat, rot);
}

void TransformLogic::GetRotation(float* x, float* y, float* z, const TransformComponent& transformComponent)
{
	if (transformComponent.RotationModeForChanges != EulerAngels)
		Logger::Warning("GetRotation called in for euler called in incorrect mode!");

	DirectX::XMFLOAT3 storedValue{};
	XMStoreFloat3(&storedValue, transformComponent.RotationVectorEuler);

	if (x != nullptr)
		*x = storedValue.x;
	if (y != nullptr)
		*y = storedValue.y;
	if (z != nullptr)
		*z = storedValue.z;
}

void TransformLogic::GetRotation(float* x, float* y, float* z, float* w, const TransformComponent& transformComponent)
{
	if (transformComponent.RotationModeForChanges != Quaternions)
		Logger::Warning("GetRotation called in for quaternions called in incorrect mode!");

	DirectX::XMFLOAT4 storedValue{};
	XMStoreFloat4(&storedValue, transformComponent.RotationVectorQuat);

	if (x != nullptr)
		*x = storedValue.x;
	if (y != nullptr)
		*y = storedValue.y;
	if (z != nullptr)
		*z = storedValue.z;
	if (w != nullptr)
		*w = storedValue.w;
}

DirectX::XMFLOAT3 TransformLogic::GetRotationEuler(const TransformComponent& transformComponent)
{
	DirectX::XMFLOAT3 storedValue{};
	XMStoreFloat3(&storedValue, transformComponent.RotationVectorEuler);
	return storedValue;
}

DirectX::XMFLOAT4 TransformLogic::GetRotationQuat(const TransformComponent& transformComponent)
{
	DirectX::XMFLOAT4 storedValue{};
	XMStoreFloat4(&storedValue, transformComponent.RotationVectorQuat);
	return storedValue;
}

void TransformLogic::LookAt(DirectX::XMFLOAT3 targetPosition, TransformComponent& transformComponent)
{
	transformComponent.ValuesChanged = true;

	DirectX::XMFLOAT3 currentCameraPosition{};
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
