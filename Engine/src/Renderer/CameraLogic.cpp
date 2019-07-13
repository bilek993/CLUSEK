#include "CameraLogic.h"
#include "../Utils/Logger.h"

void CameraLogic::SetPosition(const DirectX::XMVECTOR& pos, CameraComponent &cameraComponent, TransformComponent& transformComponent)
{
	transformComponent.PositionVector = pos;
	UpdateViewMatrix(cameraComponent, transformComponent);
}

void CameraLogic::SetPosition(const float x, const float y, const float z, CameraComponent &cameraComponent, TransformComponent& transformComponent)
{
	auto newPosition = DirectX::XMFLOAT3(x, y, z);
	transformComponent.PositionVector = XMLoadFloat3(&newPosition);
	UpdateViewMatrix(cameraComponent, transformComponent);
}

void CameraLogic::SetRotation(const DirectX::XMVECTOR& rot, CameraComponent &cameraComponent, TransformComponent& transformComponent)
{
	transformComponent.RotationVector = rot;
	UpdateViewMatrix(cameraComponent, transformComponent);
}

void CameraLogic::SetRotation(const float x, const float y, const float z, CameraComponent &cameraComponent, TransformComponent& transformComponent)
{
	auto newRotation = DirectX::XMFLOAT3(x, y, z);
	transformComponent.RotationVector = XMLoadFloat3(&newRotation);
	UpdateViewMatrix(cameraComponent, transformComponent);
}

void CameraLogic::AdjustPosition(const DirectX::XMVECTOR& pos, CameraComponent &cameraComponent, TransformComponent& transformComponent)
{
	transformComponent.PositionVector = DirectX::XMVectorAdd(transformComponent.PositionVector, pos);
	UpdateViewMatrix(cameraComponent, transformComponent);
}

void CameraLogic::AdjustPosition(const float x, const float y, const float z, CameraComponent &cameraComponent, TransformComponent& transformComponent)
{
	DirectX::XMFLOAT3 storedValue;
	XMStoreFloat3(&storedValue, transformComponent.PositionVector);
	storedValue.x += x;
	storedValue.y += y;
	storedValue.z += z;
	transformComponent.PositionVector = XMLoadFloat3(&storedValue);
	UpdateViewMatrix(cameraComponent, transformComponent);
}

void CameraLogic::AdjustRotation(const DirectX::XMVECTOR& rot, CameraComponent &cameraComponent, TransformComponent& transformComponent)
{
	transformComponent.RotationVector = DirectX::XMVectorAdd(transformComponent.RotationVector, rot);;
	UpdateViewMatrix(cameraComponent, transformComponent);
}

void CameraLogic::AdjustRotation(const float x, const float y, const float z, CameraComponent &cameraComponent, TransformComponent& transformComponent)
{
	DirectX::XMFLOAT3 storedValue;
	XMStoreFloat3(&storedValue, transformComponent.RotationVector);
	storedValue.x += x;
	storedValue.y += y;
	storedValue.z += z;
	transformComponent.RotationVector = XMLoadFloat3(&storedValue);
	UpdateViewMatrix(cameraComponent, transformComponent);
}

void CameraLogic::LookAt(DirectX::XMFLOAT3 targetPosition, CameraComponent &cameraComponent, TransformComponent& transformComponent)
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

	SetRotation(pitch, yaw, 0.0f, cameraComponent, transformComponent);
}

void CameraLogic::UpdateViewMatrix(CameraComponent& cameraComponent, TransformComponent& transformComponent)
{
	const auto cameraRotationMatrix = DirectX::XMMatrixRotationRollPitchYawFromVector(transformComponent.RotationVector);
	const auto cameraDirection = XMVector3TransformCoord(TransformComponent::FORWARD_VECTOR, cameraRotationMatrix);
	const auto cameraTarget = DirectX::XMVectorAdd(cameraDirection, transformComponent.PositionVector);
	const auto upDirection = XMVector3TransformCoord(TransformComponent::UP_VECTOR, cameraRotationMatrix);
	cameraComponent.ViewMatrix = DirectX::XMMatrixLookAtLH(transformComponent.PositionVector, cameraTarget, upDirection);

	transformComponent.VectorForward = cameraDirection;
	transformComponent.VectorRight = XMVector3TransformCoord(TransformComponent::RIGHT_VECTOR, cameraRotationMatrix);
}
