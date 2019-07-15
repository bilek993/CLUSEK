#include "CameraSystem.h"
#include "../Components/CameraComponent.h"
#include "../../Renderer/ModelViewLogic.h"
#include "../../Utils/Logger.h"
#include "../../Renderer/TransformLogic.h"

void CameraSystem::Start(entt::registry& registry, const HWND &hwnd, const ConfigData& configData)
{
	Logger::Debug("Staring camera system...");

	auto view = registry.view<CameraComponent, TransformComponent>();
	if (view.size() != 1)
	{
		if (view.size() > 1)
			Logger::Error("More than one main render camera found!");
		else
			Logger::Error("Main render camera not found!");
	}

	auto &cameraComponent = view.raw<CameraComponent>()[0];
	auto &transformComponent = view.raw<TransformComponent>()[0];

	cameraComponent.MaxRotationY = DirectX::XMConvertToRadians(configData.MaxRotationX);
	cameraComponent.MinRotationY = DirectX::XMConvertToRadians(configData.MinRotationX);

	const auto fovRadians = (configData.MainCameraFov / 360.0f) * DirectX::XM_2PI;
	cameraComponent.ProjectionMatrix = DirectX::XMMatrixPerspectiveFovLH(fovRadians,
		static_cast<float>(configData.WindowWidth) / static_cast<float>(configData.WindowHeight),
		configData.MainCameraNearZ, configData.MainCameraFarZ);

	TransformLogic::SetPosition(0.0f, 0.0f, -3.0f, transformComponent);
	ModelViewLogic::UpdateViewMatrix(cameraComponent, transformComponent);
}

void CameraSystem::Update(const float deltaTime, entt::registry &registry, IOData& ioData, IODevices &ioDevices,
	RenderWindow &window, ConfigData &configData)
{
	auto view = registry.view<CameraComponent, TransformComponent>();
	if (view.size() != 1)
	{
		if (view.size() > 1)
			Logger::Error("More than one render camera found!");
		else
			Logger::Error("Main camera not found!");
	}

	auto &cameraComponent = view.raw<CameraComponent>()[0];
	auto &transformComponent = view.raw<TransformComponent>()[0];

	if (ioData.MouseState.rightButton)
	{
		ioDevices.ChangeMouseToRelativeMode(window.GetHandle());

		auto currentRotationX = 0.0f;
		auto currentRotationY = 0.0f;
		TransformLogic::GetRotationXyz(&currentRotationX, &currentRotationY, nullptr, transformComponent);

		const auto rotationMouseX = static_cast<float>(ioData.MouseState.y) * 0.001f * deltaTime;
		const auto rotationMouseY = static_cast<float>(ioData.MouseState.x) * 0.001f * deltaTime;

		if (currentRotationX + rotationMouseX < cameraComponent.MaxRotationY && currentRotationX + rotationMouseX > cameraComponent.MinRotationY)
			currentRotationX += rotationMouseX;

		TransformLogic::SetRotation(currentRotationX, currentRotationY + rotationMouseY, 0.0f, transformComponent);
		ModelViewLogic::UpdateViewMatrix(cameraComponent, transformComponent);
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
		TransformLogic::AdjustPosition(DirectX::XMVectorScale(transformComponent.VectorForward, cameraSpeed), transformComponent);
		ModelViewLogic::UpdateViewMatrix(cameraComponent, transformComponent);
	}
	if (ioData.KeyboardState.A)
	{
		TransformLogic::AdjustPosition(DirectX::XMVectorScale(transformComponent.VectorRight, -cameraSpeed), transformComponent);
		ModelViewLogic::UpdateViewMatrix(cameraComponent, transformComponent);
	}
	if (ioData.KeyboardState.S)
	{
		TransformLogic::AdjustPosition(DirectX::XMVectorScale(transformComponent.VectorForward, -cameraSpeed), transformComponent);
		ModelViewLogic::UpdateViewMatrix(cameraComponent, transformComponent);
	}
	if (ioData.KeyboardState.D)
	{
		TransformLogic::AdjustPosition(DirectX::XMVectorScale(transformComponent.VectorRight, cameraSpeed), transformComponent);
		ModelViewLogic::UpdateViewMatrix(cameraComponent, transformComponent);
	}
}