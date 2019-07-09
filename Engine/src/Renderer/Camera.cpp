#include "Camera.h"
#include "../Utils/Logger.h"

const DirectX::XMVECTOR Camera::FORWARD_VECTOR = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
const DirectX::XMVECTOR Camera::UP_VECTOR = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
const DirectX::XMVECTOR Camera::RIGHT_VECTOR = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);

Camera::Camera()
{
	Logger::Debug("Creating new instance of camera...");
	PositionVector = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	RotationVector = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	UpdateViewMatrix();
}

void Camera::SetCameraSettings(const float fov, const float aspectRatio, const float nearZ, const float farZ)
{
	const auto fovRadians = (fov / 360.0f) * DirectX::XM_2PI;
	ProjectionMatrix = DirectX::XMMatrixPerspectiveFovLH(fovRadians, aspectRatio, nearZ, farZ);
	Logger::Debug("Camera settings have been changed.");
}

const DirectX::XMMATRIX& Camera::GetViewMatrix() const
{
	return ViewMatrix;
}

const DirectX::XMMATRIX& Camera::GetProjectionMatrix() const
{
	return ProjectionMatrix;
}

const DirectX::XMVECTOR& Camera::GetPositionVector() const
{
	return PositionVector;
}

const DirectX::XMVECTOR& Camera::GetRotationVector() const
{
	return RotationVector;
}

const DirectX::XMVECTOR& Camera::GetForwardVector() const
{
	return VectorForward;
}

const DirectX::XMVECTOR& Camera::GetRightVector() const
{
	return VectorRight;
}

DirectX::XMFLOAT3 Camera::GetPositionFloat3() const
{
	DirectX::XMFLOAT3 returnedValue;
	XMStoreFloat3(&returnedValue, PositionVector);
	return returnedValue;
}

DirectX::XMFLOAT3 Camera::GetRotationFloat3() const
{
	DirectX::XMFLOAT3 returnedValue;
	XMStoreFloat3(&returnedValue, RotationVector);
	return returnedValue;
}

void Camera::SetPosition(const DirectX::XMVECTOR& pos)
{
	PositionVector = pos;
	UpdateViewMatrix();
}

void Camera::SetPosition(const float x, const float y, const float z)
{
	auto newPosition = DirectX::XMFLOAT3(x, y, z);
	PositionVector = XMLoadFloat3(&newPosition);
	UpdateViewMatrix();
}

void Camera::SetRotation(const DirectX::XMVECTOR& rot)
{
	RotationVector = rot;
	UpdateViewMatrix();
}

void Camera::SetRotation(const float x, const float y, const float z)
{
	auto newRotation = DirectX::XMFLOAT3(x, y, z);
	RotationVector = XMLoadFloat3(&newRotation);
	UpdateViewMatrix();
}

void Camera::AdjustPosition(const DirectX::XMVECTOR& pos)
{
	PositionVector = DirectX::XMVectorAdd(PositionVector, pos);
	UpdateViewMatrix();
}

void Camera::AdjustPosition(const float x, const float y, const float z)
{
	DirectX::XMFLOAT3 storedValue;
	XMStoreFloat3(&storedValue, PositionVector);
	storedValue.x += x;
	storedValue.y += y;
	storedValue.z += z;
	PositionVector = XMLoadFloat3(&storedValue);
	UpdateViewMatrix();
}

void Camera::AdjustRotation(const DirectX::XMVECTOR& rot)
{
	RotationVector = DirectX::XMVectorAdd(RotationVector, rot);;
	UpdateViewMatrix();
}

void Camera::AdjustRotation(const float x, const float y, const float z)
{
	DirectX::XMFLOAT3 storedValue;
	XMStoreFloat3(&storedValue, RotationVector);
	storedValue.x += x;
	storedValue.y += y;
	storedValue.z += z;
	RotationVector = XMLoadFloat3(&storedValue);
	UpdateViewMatrix();
}

void Camera::LookAt(DirectX::XMFLOAT3 targetPosition)
{
	DirectX::XMFLOAT3 currentCameraPosition;
	XMStoreFloat3(&currentCameraPosition, PositionVector);

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

	SetRotation(pitch, yaw, 0.0f);
}

void Camera::UpdateViewMatrix()
{
	const auto cameraRotationMatrix = DirectX::XMMatrixRotationRollPitchYawFromVector(RotationVector);
	const auto cameraDirection = XMVector3TransformCoord(FORWARD_VECTOR, cameraRotationMatrix);
	const auto cameraTarget = DirectX::XMVectorAdd(cameraDirection, PositionVector);
	const auto upDirection = XMVector3TransformCoord(UP_VECTOR, cameraRotationMatrix);
	ViewMatrix = DirectX::XMMatrixLookAtLH(this->PositionVector, cameraTarget, upDirection);

	VectorForward = cameraDirection;
	VectorRight = XMVector3TransformCoord(RIGHT_VECTOR, cameraRotationMatrix);
}
