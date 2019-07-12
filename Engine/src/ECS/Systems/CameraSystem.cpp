#include "CameraSystem.h"
#include "../Components/CameraComponent.h"
#include "../../Renderer/CameraLogic.h"

void CameraSystem::Start(entt::registry& registry, const HWND &hwnd, const ConfigData& configData, Renderer &renderer)
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

	CameraLogic::SetPosition(0.0f, 0.0f, -3.0f, cameraComponent);
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
		CameraLogic::AdjustRotation(static_cast<float>(ioData.MouseState.y) * 0.001f * deltaTime,
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
		CameraLogic::AdjustPosition(DirectX::XMVectorScale(cameraComponent.VectorForward, cameraSpeed), cameraComponent);
	}
	if (ioData.KeyboardState.A)
	{
		CameraLogic::AdjustPosition(DirectX::XMVectorScale(cameraComponent.VectorRight, -cameraSpeed), cameraComponent);
	}
	if (ioData.KeyboardState.S)
	{
		CameraLogic::AdjustPosition(DirectX::XMVectorScale(cameraComponent.VectorForward, -cameraSpeed), cameraComponent);
	}
	if (ioData.KeyboardState.D)
	{
		CameraLogic::AdjustPosition(DirectX::XMVectorScale(cameraComponent.VectorRight, cameraSpeed), cameraComponent);
	}
}