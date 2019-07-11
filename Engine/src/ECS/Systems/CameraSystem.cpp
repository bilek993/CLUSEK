#include "CameraSystem.h"
#include "../Components/CameraComponent.h"

void CameraSystem::Start(entt::registry& registry, const ConfigData& configData, Renderer &renderer)
{
	auto view = registry.view<CameraComponent>();
	if (view.size() != 1)
	{
		if (view.size() > 1)
			Logger::Error("More than one main render camera found!");
		else
			Logger::Error("Main render camera not found!");
	}

	auto &cameraComponent = view.raw()[0];

	const auto fovRadians = (configData.MainCameraFov / 360.0f) * DirectX::XM_2PI;
	cameraComponent.ProjectionMatrix = DirectX::XMMatrixPerspectiveFovLH(fovRadians,
		static_cast<float>(configData.WindowWidth) / static_cast<float>(configData.WindowHeight),
		configData.MainCameraNearZ, configData.MainCameraFarZ);

	SetPosition(0.0f, 0.0f, -3.0f, cameraComponent);
}

void CameraSystem::Update(const float deltaTime, entt::registry &registry, IOData& ioData, IODevices &ioDevices,
	Renderer &renderer, RenderWindow &window, ConfigData &configData)
{
	auto view = registry.view<CameraComponent>();
	if (view.size() != 1)
	{
		if (view.size() > 1)
			Logger::Error("More than one render camera found!");
		else
			Logger::Error("Main camera not found!");
	}

	auto &cameraComponent = view.raw()[0];

	if (ioData.MouseState.rightButton)
	{
		ioDevices.ChangeMouseToRelativeMode(window.GetHandle());
		AdjustRotation(static_cast<float>(ioData.MouseState.y) * 0.001f * deltaTime,
			static_cast<float>(ioData.MouseState.x) * 0.001f * deltaTime, 0.0f, cameraComponent);
	}
	else
	{
		ioDevices.ChangeMouseToAbsoluteMode(window.GetHandle());
	}

	auto cameraSpeed = 0.0025f * deltaTime;
	if (ioData.KeyboardState.LeftShift || ioData.KeyboardState.RightShift)
		cameraSpeed *= 5;

	if (ioData.KeyboardState.W)
	{
		AdjustPosition(DirectX::XMVectorScale(cameraComponent.VectorForward, cameraSpeed), cameraComponent);
	}
	if (ioData.KeyboardState.A)
	{
		AdjustPosition(DirectX::XMVectorScale(cameraComponent.VectorRight, -cameraSpeed), cameraComponent);
	}
	if (ioData.KeyboardState.S)
	{
		AdjustPosition(DirectX::XMVectorScale(cameraComponent.VectorForward, -cameraSpeed), cameraComponent);
	}
	if (ioData.KeyboardState.D)
	{
		AdjustPosition(DirectX::XMVectorScale(cameraComponent.VectorRight, cameraSpeed), cameraComponent);
	}
}

void CameraSystem::SetPosition(const DirectX::XMVECTOR& pos, CameraComponent& cameraComponent)
{
	cameraComponent.PositionVector = pos;
	UpdateViewMatrix(cameraComponent);
}

void CameraSystem::SetPosition(const float x, const float y, const float z, CameraComponent& cameraComponent)
{
	auto newPosition = DirectX::XMFLOAT3(x, y, z);
	cameraComponent.PositionVector = XMLoadFloat3(&newPosition);
	UpdateViewMatrix(cameraComponent);
}

void CameraSystem::SetRotation(const DirectX::XMVECTOR& rot, CameraComponent& cameraComponent)
{
	cameraComponent.RotationVector = rot;
	UpdateViewMatrix(cameraComponent);
}

void CameraSystem::SetRotation(const float x, const float y, const float z, CameraComponent& cameraComponent)
{
	auto newRotation = DirectX::XMFLOAT3(x, y, z);
	cameraComponent.RotationVector = XMLoadFloat3(&newRotation);
	UpdateViewMatrix(cameraComponent);
}

void CameraSystem::AdjustPosition(const DirectX::XMVECTOR& pos, CameraComponent& cameraComponent)
{
	cameraComponent.PositionVector = DirectX::XMVectorAdd(cameraComponent.PositionVector, pos);
	UpdateViewMatrix(cameraComponent);
}

void CameraSystem::AdjustPosition(const float x, const float y, const float z, CameraComponent& cameraComponent)
{
	DirectX::XMFLOAT3 storedValue;
	XMStoreFloat3(&storedValue, cameraComponent.PositionVector);
	storedValue.x += x;
	storedValue.y += y;
	storedValue.z += z;
	cameraComponent.PositionVector = XMLoadFloat3(&storedValue);
	UpdateViewMatrix(cameraComponent);
}

void CameraSystem::AdjustRotation(const DirectX::XMVECTOR& rot, CameraComponent& cameraComponent)
{
	cameraComponent.RotationVector = DirectX::XMVectorAdd(cameraComponent.RotationVector, rot);;
	UpdateViewMatrix(cameraComponent);
}

void CameraSystem::AdjustRotation(const float x, const float y, const float z, CameraComponent& cameraComponent)
{
	DirectX::XMFLOAT3 storedValue;
	XMStoreFloat3(&storedValue, cameraComponent.RotationVector);
	storedValue.x += x;
	storedValue.y += y;
	storedValue.z += z;
	cameraComponent.RotationVector = XMLoadFloat3(&storedValue);
	UpdateViewMatrix(cameraComponent);
}

void CameraSystem::LookAt(DirectX::XMFLOAT3 targetPosition, CameraComponent& cameraComponent)
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

void CameraSystem::UpdateViewMatrix(CameraComponent& cameraComponent)
{
	const auto cameraRotationMatrix = DirectX::XMMatrixRotationRollPitchYawFromVector(cameraComponent.RotationVector);
	const auto cameraDirection = XMVector3TransformCoord(CameraComponent::FORWARD_VECTOR, cameraRotationMatrix);
	const auto cameraTarget = DirectX::XMVectorAdd(cameraDirection, cameraComponent.PositionVector);
	const auto upDirection = XMVector3TransformCoord(CameraComponent::UP_VECTOR, cameraRotationMatrix);
	cameraComponent.ViewMatrix = DirectX::XMMatrixLookAtLH(cameraComponent.PositionVector, cameraTarget, upDirection);

	cameraComponent.VectorForward = cameraDirection;
	cameraComponent.VectorRight = XMVector3TransformCoord(CameraComponent::RIGHT_VECTOR, cameraRotationMatrix);
}
