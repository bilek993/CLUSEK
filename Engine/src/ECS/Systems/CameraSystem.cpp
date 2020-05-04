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
	const auto cameraSpeed = GetCameraSpeed(deltaTime);
	const auto cameraTranslation = GetTranslation(cameraSpeed, cameraComponent);
	const auto cameraRotation = GetRotation(deltaTime, cameraComponent, transformComponent);

	TransformLogic::AdjustPosition(cameraTranslation, transformComponent);
	TransformLogic::SetRotationEuler(cameraRotation.first, cameraRotation.second, 0.0f, transformComponent);

	UpdateViewMatrix(cameraComponent, transformComponent);
}

float CameraSystem::GetCameraSpeed(const float deltaTime) const
{
	auto cameraSpeed = ConfigurationData->MaxCameraSpeed / 5 * deltaTime;

	if (InputOutputData->GamePadState.IsConnected())
	{
		if (InputOutputData->GamePadState.IsRightStickPressed())
			cameraSpeed *= 5;
	}
	else
	{
		if (InputOutputData->KeyboardState.LeftShift || InputOutputData->KeyboardState.RightShift)
			cameraSpeed *= 5;
	}

	return cameraSpeed;
}

bool CameraSystem::GetTargetMode(const CameraComponent& cameraComponent) const
{
	auto useTarget = cameraComponent.UseTarget;

	if (InputOutputData->GamePadState.IsConnected())
	{
		if (InputOutputData->GamePadState.IsXPressed())
			useTarget = !useTarget;
	}
	else
	{
		if (InputOutputData->KeyboardState.C)
			useTarget = !useTarget;
	}

	return useTarget;
}

DirectX::XMVECTOR CameraSystem::GetTranslation(const float cameraSpeed, const CameraComponent& cameraComponent) const
{
	DirectX::XMVECTOR translationVector{};

	if (InputOutputData->GamePadState.IsConnected())
	{
		if (InputOutputData->GamePadState.dpad.up)
			translationVector = DirectX::XMVectorAdd(DirectX::XMVectorScale(cameraComponent.VectorForward, cameraSpeed), translationVector);
		if (InputOutputData->GamePadState.dpad.left)
			translationVector = DirectX::XMVectorAdd(DirectX::XMVectorScale(cameraComponent.VectorRight, -cameraSpeed), translationVector);
		if (InputOutputData->GamePadState.dpad.down)
			translationVector = DirectX::XMVectorAdd(DirectX::XMVectorScale(cameraComponent.VectorForward, -cameraSpeed), translationVector);
		if (InputOutputData->GamePadState.dpad.right)
			translationVector = DirectX::XMVectorAdd(DirectX::XMVectorScale(cameraComponent.VectorRight, cameraSpeed), translationVector);
	}
	else
	{
		if (InputOutputData->KeyboardState.W)
			translationVector = DirectX::XMVectorAdd(DirectX::XMVectorScale(cameraComponent.VectorForward, cameraSpeed), translationVector);
		if (InputOutputData->KeyboardState.A)
			translationVector = DirectX::XMVectorAdd(DirectX::XMVectorScale(cameraComponent.VectorRight, -cameraSpeed), translationVector);
		if (InputOutputData->KeyboardState.S)
			translationVector = DirectX::XMVectorAdd(DirectX::XMVectorScale(cameraComponent.VectorForward, -cameraSpeed), translationVector);
		if (InputOutputData->KeyboardState.D)
			translationVector = DirectX::XMVectorAdd(DirectX::XMVectorScale(cameraComponent.VectorRight, cameraSpeed), translationVector);
	}

	return translationVector;
}

std::pair<float, float> CameraSystem::GetRotation(const float deltaTime, const CameraComponent& cameraComponent, const TransformComponent& transformComponent) const
{
	auto rotationX = 0.0f;
	auto rotationY = 0.0f;
	TransformLogic::GetRotation(&rotationX, &rotationY, nullptr, transformComponent);

	if (InputOutputData->GamePadState.IsConnected())
	{
		rotationX += static_cast<float>(-InputOutputData->GamePadState.thumbSticks.rightY) * 0.002f * deltaTime;
		rotationY += static_cast<float>(InputOutputData->GamePadState.thumbSticks.rightX) * 0.002f * deltaTime;
	}
	else
	{
		if (InputOutputData->MouseState.rightButton)
		{
			InputOutputDevices->ChangeMouseToRelativeMode(Window->GetHandle());

			rotationX += static_cast<float>(InputOutputData->MouseState.y) * 0.001f * deltaTime;
			rotationY += static_cast<float>(InputOutputData->MouseState.x) * 0.001f * deltaTime;
		}
		else
		{
			InputOutputDevices->ChangeMouseToAbsoluteMode(Window->GetHandle());
		}
	}

	if (rotationX > cameraComponent.MaxRotationY)
		rotationX = cameraComponent.MaxRotationY;
	else if (rotationX < cameraComponent.MinRotationY)
		rotationX = cameraComponent.MinRotationY;

	return std::pair<float, float>(rotationX, rotationY);
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
