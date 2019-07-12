#include "CameraLogic.h"
#include "../Utils/Logger.h"

void CameraLogic::SetPosition(const DirectX::XMVECTOR& pos, CameraComponent& cameraComponent)
{
	cameraComponent.PositionVector = pos;
	UpdateViewMatrix(cameraComponent);
}

void CameraLogic::SetPosition(const float x, const float y, const float z, CameraComponent& cameraComponent)
{
	auto newPosition = DirectX::XMFLOAT3(x, y, z);
	cameraComponent.PositionVector = XMLoadFloat3(&newPosition);
	UpdateViewMatrix(cameraComponent);
}

void CameraLogic::SetRotation(const DirectX::XMVECTOR& rot, CameraComponent& cameraComponent)
{
	cameraComponent.RotationVector = rot;
	UpdateViewMatrix(cameraComponent);
}

void CameraLogic::SetRotation(const float x, const float y, const float z, CameraComponent& cameraComponent)
{
	auto newRotation = DirectX::XMFLOAT3(x, y, z);
	cameraComponent.RotationVector = XMLoadFloat3(&newRotation);
	UpdateViewMatrix(cameraComponent);
}

void CameraLogic::AdjustPosition(const DirectX::XMVECTOR& pos, CameraComponent& cameraComponent)
{
	cameraComponent.PositionVector = DirectX::XMVectorAdd(cameraComponent.PositionVector, pos);
	UpdateViewMatrix(cameraComponent);
}

void CameraLogic::AdjustPosition(const float x, const float y, const float z, CameraComponent& cameraComponent)
{
	DirectX::XMFLOAT3 storedValue;
	XMStoreFloat3(&storedValue, cameraComponent.PositionVector);
	storedValue.x += x;
	storedValue.y += y;
	storedValue.z += z;
	cameraComponent.PositionVector = XMLoadFloat3(&storedValue);
	UpdateViewMatrix(cameraComponent);
}

void CameraLogic::AdjustRotation(const DirectX::XMVECTOR& rot, CameraComponent& cameraComponent)
{
	cameraComponent.RotationVector = DirectX::XMVectorAdd(cameraComponent.RotationVector, rot);;
	UpdateViewMatrix(cameraComponent);
}

void CameraLogic::AdjustRotation(const float x, const float y, const float z, CameraComponent& cameraComponent)
{
	DirectX::XMFLOAT3 storedValue;
	XMStoreFloat3(&storedValue, cameraComponent.RotationVector);
	storedValue.x += x;
	storedValue.y += y;
	storedValue.z += z;
	cameraComponent.RotationVector = XMLoadFloat3(&storedValue);
	UpdateViewMatrix(cameraComponent);
}

void CameraLogic::LookAt(DirectX::XMFLOAT3 targetPosition, CameraComponent& cameraComponent)
{
	DirectX::XMFLOAT3 currentCameraPosition;
	XMStoreFloat3(&currentCameraPosition, cameraComponent.PositionVector);

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

	SetRotation(pitch, yaw, 0.0f, cameraComponent);
}

void CameraLogic::UpdateViewMatrix(CameraComponent& cameraComponent)
{
	const auto cameraRotationMatrix = DirectX::XMMatrixRotationRollPitchYawFromVector(cameraComponent.RotationVector);
	const auto cameraDirection = XMVector3TransformCoord(CameraComponent::FORWARD_VECTOR, cameraRotationMatrix);
	const auto cameraTarget = DirectX::XMVectorAdd(cameraDirection, cameraComponent.PositionVector);
	const auto upDirection = XMVector3TransformCoord(CameraComponent::UP_VECTOR, cameraRotationMatrix);
	cameraComponent.ViewMatrix = DirectX::XMMatrixLookAtLH(cameraComponent.PositionVector, cameraTarget, upDirection);

	cameraComponent.VectorForward = cameraDirection;
	cameraComponent.VectorRight = XMVector3TransformCoord(CameraComponent::RIGHT_VECTOR, cameraRotationMatrix);
}
