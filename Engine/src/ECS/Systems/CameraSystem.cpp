#include "CameraSystem.h"
#include "../Components/CameraComponent.h"
#include "../../Renderer/CameraLogic.h"
#include "../../Utils/Logger.h"

void CameraSystem::Start(entt::registry& registry, const HWND &hwnd, const ConfigData& configData)
{
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

	const auto fovRadians = (configData.MainCameraFov / 360.0f) * DirectX::XM_2PI;
	cameraComponent.ProjectionMatrix = DirectX::XMMatrixPerspectiveFovLH(fovRadians,
		static_cast<float>(configData.WindowWidth) / static_cast<float>(configData.WindowHeight),
		configData.MainCameraNearZ, configData.MainCameraFarZ);

	CameraLogic::SetPosition(0.0f, 0.0f, -3.0f, cameraComponent, transformComponent);
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
		CameraLogic::AdjustRotation(static_cast<float>(ioData.MouseState.y) * 0.001f * deltaTime,
			static_cast<float>(ioData.MouseState.x) * 0.001f * deltaTime, 0.0f, cameraComponent, transformComponent);
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
		CameraLogic::AdjustPosition(DirectX::XMVectorScale(transformComponent.VectorForward, cameraSpeed), cameraComponent, transformComponent);
	}
	if (ioData.KeyboardState.A)
	{
		CameraLogic::AdjustPosition(DirectX::XMVectorScale(transformComponent.VectorRight, -cameraSpeed), cameraComponent, transformComponent);
	}
	if (ioData.KeyboardState.S)
	{
		CameraLogic::AdjustPosition(DirectX::XMVectorScale(transformComponent.VectorForward, -cameraSpeed), cameraComponent, transformComponent);
	}
	if (ioData.KeyboardState.D)
	{
		CameraLogic::AdjustPosition(DirectX::XMVectorScale(transformComponent.VectorRight, cameraSpeed), cameraComponent, transformComponent);
	}
}