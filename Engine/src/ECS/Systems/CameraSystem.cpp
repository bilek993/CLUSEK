#include "CameraSystem.h"
#include "../Components/CameraComponent.h"
#include "../../Renderer/ModelViewLogic.h"
#include "../../Utils/Logger.h"
#include "../../Renderer/TransformLogic.h"

void CameraSystem::Start()
{
	Logger::Debug("Staring camera system...");

	auto view = Registry->view<CameraComponent, TransformComponent>();
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

	ModelViewLogic::UpdateViewMatrix(cameraComponent, transformComponent);
}

void CameraSystem::Update(const float deltaTime)
{
	auto view = Registry->view<CameraComponent, TransformComponent>();
	if (view.size() != 1)
	{
		if (view.size() > 1)
			Logger::Error("More than one render camera found!");
		else
			Logger::Error("Main camera not found!");
	}

	auto &cameraComponent = view.raw<CameraComponent>()[0];
	auto &transformComponent = view.raw<TransformComponent>()[0];

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

		TransformLogic::SetRotation(currentRotationX, currentRotationY + rotationMouseY, 0.0f, transformComponent);
		ModelViewLogic::UpdateViewMatrix(cameraComponent, transformComponent);
	}
	else
	{
		InputOutputDevices->ChangeMouseToAbsoluteMode(Window->GetHandle());
	}

	auto cameraSpeed = 0.0025f * deltaTime;
	if (InputOutputData->KeyboardState.LeftShift || InputOutputData->KeyboardState.RightShift)
		cameraSpeed *= 5;

	if (InputOutputData->KeyboardState.W)
	{
		TransformLogic::AdjustPosition(DirectX::XMVectorScale(transformComponent.VectorForward, cameraSpeed), transformComponent);
		ModelViewLogic::UpdateViewMatrix(cameraComponent, transformComponent);
	}
	if (InputOutputData->KeyboardState.A)
	{
		TransformLogic::AdjustPosition(DirectX::XMVectorScale(transformComponent.VectorRight, -cameraSpeed), transformComponent);
		ModelViewLogic::UpdateViewMatrix(cameraComponent, transformComponent);
	}
	if (InputOutputData->KeyboardState.S)
	{
		TransformLogic::AdjustPosition(DirectX::XMVectorScale(transformComponent.VectorForward, -cameraSpeed), transformComponent);
		ModelViewLogic::UpdateViewMatrix(cameraComponent, transformComponent);
	}
	if (InputOutputData->KeyboardState.D)
	{
		TransformLogic::AdjustPosition(DirectX::XMVectorScale(transformComponent.VectorRight, cameraSpeed), transformComponent);
		ModelViewLogic::UpdateViewMatrix(cameraComponent, transformComponent);
	}
}