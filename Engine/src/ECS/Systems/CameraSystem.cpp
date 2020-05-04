#include "CameraSystem.h"
#include "../Components/CameraComponent.h"
#include "../../Utils/Logger.h"
#include "../../Renderer/TransformLogic.h"
#include "../../Tags.h"
#include "../../Renderer/FrustumUtil.h"

void CameraSystem::Start()
{
	Logger::Debug("Staring camera system...");

	auto view = Registry->view<CameraComponent, TransformComponent, entt::tag<Tags::MAIN_CAMERA>>();
	if (view.size() != 1)
	{
		if (view.size() > 1)
			Logger::Error("More than one main render camera found!");
		else
			Logger::Error("Main render camera not found!");
	}

	auto &cameraComponent = view.raw<CameraComponent>()[0];
	auto &transformComponent = view.raw<TransformComponent>()[0];

	cameraComponent.MaxRotationY = DirectX::XMConvertToRadians(ConfigurationData->MaxRotationX);
	cameraComponent.MinRotationY = DirectX::XMConvertToRadians(ConfigurationData->MinRotationX);

	const auto fovRadians = (ConfigurationData->MainCameraFov / 360.0f) * DirectX::XM_2PI;
	cameraComponent.ProjectionMatrix = DirectX::XMMatrixPerspectiveFovLH(fovRadians,
		static_cast<float>(ConfigurationData->WindowWidth) / static_cast<float>(ConfigurationData->WindowHeight),
		ConfigurationData->MainCameraNearZ, ConfigurationData->MainCameraFarZ);

	UpdateViewMatrix(cameraComponent, transformComponent);
}

void CameraSystem::Update(const float deltaTime)
{
	auto view = Registry->view<CameraComponent, TransformComponent, entt::tag<Tags::MAIN_CAMERA>>();
	if (view.size() != 1)
	{
		if (view.size() > 1)
			Logger::Error("More than one render camera found!");
		else
			Logger::Error("Main camera not found!");
	}

	auto &cameraComponent = view.raw<CameraComponent>()[0];
	auto &transformComponent = view.raw<TransformComponent>()[0];
	
	HandleMovement(deltaTime, cameraComponent, transformComponent);

	const auto viewProjectionMatrix = cameraComponent.ViewMatrix * cameraComponent.ProjectionMatrix;
	cameraComponent.FrustumPlanes = FrustumUtil::CalculateFrustumPlanes(viewProjectionMatrix);
}

void CameraSystem::HandleMovement(const float deltaTime, CameraComponent& cameraComponent, TransformComponent& transformComponent) const
{
	if (InputOutputData->GamePadState.IsConnected())
	{
		GamepadMovement(deltaTime, cameraComponent, transformComponent);
	}
	else
	{
		KeyboardMovement(deltaTime, cameraComponent, transformComponent);
		MouseMovement(deltaTime, cameraComponent, transformComponent);
	}
}

void CameraSystem::GamepadMovement(const float deltaTime, CameraComponent& cameraComponent,
	TransformComponent& transformComponent) const
{
	auto cameraSpeed = ConfigurationData->MaxCameraSpeed / 5 * deltaTime;
	if (InputOutputData->GamePadState.IsRightStickPressed())
		cameraSpeed *= 5;

	if (InputOutputData->GamePadState.IsXPressed())
		cameraComponent.UseTarget = !cameraComponent.UseTarget;

	if (InputOutputData->GamePadState.dpad.up)
		TransformLogic::AdjustPosition(DirectX::XMVectorScale(cameraComponent.VectorForward, cameraSpeed), transformComponent);
	if (InputOutputData->GamePadState.dpad.left)
		TransformLogic::AdjustPosition(DirectX::XMVectorScale(cameraComponent.VectorRight, -cameraSpeed), transformComponent);
	if (InputOutputData->GamePadState.dpad.down)
		TransformLogic::AdjustPosition(DirectX::XMVectorScale(cameraComponent.VectorForward, -cameraSpeed), transformComponent);
	if (InputOutputData->GamePadState.dpad.right)
		TransformLogic::AdjustPosition(DirectX::XMVectorScale(cameraComponent.VectorRight, cameraSpeed), transformComponent);

	auto currentRotationX = 0.0f;
	auto currentRotationY = 0.0f;
	TransformLogic::GetRotation(&currentRotationX, &currentRotationY, nullptr, transformComponent);

	const auto rotationGamePadX = static_cast<float>(-InputOutputData->GamePadState.thumbSticks.rightY) * 0.002f * deltaTime;
	const auto rotationGamePadY = static_cast<float>(InputOutputData->GamePadState.thumbSticks.rightX) * 0.002f * deltaTime;

	if (currentRotationX + rotationGamePadX < cameraComponent.MaxRotationY && currentRotationX + rotationGamePadX > cameraComponent.MinRotationY)
		currentRotationX += rotationGamePadX;

	TransformLogic::SetRotationEuler(currentRotationX, currentRotationY + rotationGamePadY, 0.0f, transformComponent);

	UpdateViewMatrix(cameraComponent, transformComponent);
}

void CameraSystem::KeyboardMovement(const float deltaTime, CameraComponent& cameraComponent,
	TransformComponent& transformComponent) const
{
	auto cameraSpeed = ConfigurationData->MaxCameraSpeed / 5 * deltaTime;
	if (InputOutputData->KeyboardState.LeftShift || InputOutputData->KeyboardState.RightShift)
		cameraSpeed *= 5;

	if (InputOutputData->KeyboardState.C)
		cameraComponent.UseTarget = !cameraComponent.UseTarget;

	if (InputOutputData->KeyboardState.W)
	{
		TransformLogic::AdjustPosition(DirectX::XMVectorScale(cameraComponent.VectorForward, cameraSpeed), transformComponent);
		UpdateViewMatrix(cameraComponent, transformComponent);
	}
	if (InputOutputData->KeyboardState.A)
	{
		TransformLogic::AdjustPosition(DirectX::XMVectorScale(cameraComponent.VectorRight, -cameraSpeed), transformComponent);
		UpdateViewMatrix(cameraComponent, transformComponent);
	}
	if (InputOutputData->KeyboardState.S)
	{
		TransformLogic::AdjustPosition(DirectX::XMVectorScale(cameraComponent.VectorForward, -cameraSpeed), transformComponent);
		UpdateViewMatrix(cameraComponent, transformComponent);
	}
	if (InputOutputData->KeyboardState.D)
	{
		TransformLogic::AdjustPosition(DirectX::XMVectorScale(cameraComponent.VectorRight, cameraSpeed), transformComponent);
		UpdateViewMatrix(cameraComponent, transformComponent);
	}
}

void CameraSystem::MouseMovement(const float deltaTime, CameraComponent& cameraComponent,
	TransformComponent& transformComponent) const
{
	if (InputOutputData->MouseState.rightButton)
	{
		InputOutputDevices->ChangeMouseToRelativeMode(Window->GetHandle());

		auto currentRotationX = 0.0f;
		auto currentRotationY = 0.0f;
		TransformLogic::GetRotation(&currentRotationX, &currentRotationY, nullptr, transformComponent);

		const auto rotationMouseX = static_cast<float>(InputOutputData->MouseState.y) * 0.001f * deltaTime;
		const auto rotationMouseY = static_cast<float>(InputOutputData->MouseState.x) * 0.001f * deltaTime;

		if (currentRotationX + rotationMouseX < cameraComponent.MaxRotationY && currentRotationX + rotationMouseX > cameraComponent.MinRotationY)
			currentRotationX += rotationMouseX;

		TransformLogic::SetRotationEuler(currentRotationX, currentRotationY + rotationMouseY, 0.0f, transformComponent);
		UpdateViewMatrix(cameraComponent, transformComponent);
	}
	else
	{
		InputOutputDevices->ChangeMouseToAbsoluteMode(Window->GetHandle());
	}
}

void CameraSystem::UpdateViewMatrix(CameraComponent& cameraComponent, TransformComponent& transformComponent) const
{
	const auto position = TransformLogic::GetPosition(transformComponent);
	const auto positionVector = XMLoadFloat3(&position);

	const auto rotation = TransformLogic::GetRotationEuler(transformComponent);
	const auto rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);

	const auto cameraDirection = XMVector3TransformCoord(TransformComponent::FORWARD_VECTOR, rotationMatrix);
	const auto cameraTarget = DirectX::XMVectorAdd(cameraDirection, positionVector);
	const auto upDirection = XMVector3TransformCoord(TransformComponent::UP_VECTOR, rotationMatrix);

	cameraComponent.ViewMatrix = DirectX::XMMatrixLookAtLH(positionVector, cameraTarget, upDirection);

	cameraComponent.VectorForward = cameraDirection;
	cameraComponent.VectorRight = XMVector3TransformCoord(TransformComponent::RIGHT_VECTOR, rotationMatrix);
}
