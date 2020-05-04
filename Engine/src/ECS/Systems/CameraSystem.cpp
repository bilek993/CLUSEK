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
	// Get main camera data
	auto viewMainCamera = Registry->view<CameraComponent, TransformComponent, entt::tag<Tags::MAIN_CAMERA>>();
	if (viewMainCamera.size() != 1)
	{
		if (viewMainCamera.size() > 1)
			Logger::Error("More than one render camera found!");
		else
			Logger::Error("Main camera not found!");
	}

	auto &mainCameraCameraComponent = viewMainCamera.raw<CameraComponent>()[0];
	auto &mainCameraTransformComponent = viewMainCamera.raw<TransformComponent>()[0];

	// Get target camera data
	auto viewCameraTarget = Registry->view<CameraTargetComponent, TransformComponent>();

	CameraTargetComponent* cameraTargetComponent = nullptr;
	TransformComponent* cameraTargetTransformComponent = nullptr;

	for (auto& entity : viewCameraTarget)
	{
		auto &currentCameraTarget = viewCameraTarget.get<CameraTargetComponent>(entity);
		auto &currentTargetTransform = viewCameraTarget.get<TransformComponent>(entity);

		if (currentCameraTarget.Active)
		{
			cameraTargetComponent = &currentCameraTarget;
			cameraTargetTransformComponent = &currentTargetTransform;
			break;
		}
	}

	// Perform calculations
	HandleMovement(	deltaTime, 
					mainCameraCameraComponent, 
					mainCameraTransformComponent,
					cameraTargetComponent, 
					cameraTargetTransformComponent);

	const auto viewProjectionMatrix = mainCameraCameraComponent.ViewMatrix * mainCameraCameraComponent.ProjectionMatrix;
	mainCameraCameraComponent.FrustumPlanes = FrustumUtil::CalculateFrustumPlanes(viewProjectionMatrix);
}

void CameraSystem::HandleMovement(const float deltaTime, CameraComponent& mainCameraCameraComponent, TransformComponent& mainCameraTransformComponent,
	const CameraTargetComponent* cameraTargetComponent, const TransformComponent* cameraTargetTransformComponent) const
{
	const auto cameraSpeed = GetCameraSpeed(deltaTime);
	const auto cameraTranslation = GetTranslation(cameraSpeed, mainCameraCameraComponent);
	const auto cameraRotation = GetRotation(deltaTime, mainCameraCameraComponent, mainCameraTransformComponent);
	const auto useTargetMovement = GetTargetMode(mainCameraCameraComponent) && !cameraTargetComponent && !cameraTargetTransformComponent;

	TransformLogic::AdjustPosition(cameraTranslation, mainCameraTransformComponent);
	TransformLogic::SetRotationEuler(cameraRotation.first, cameraRotation.second, 0.0f, mainCameraTransformComponent);

	UpdateViewMatrix(mainCameraCameraComponent, mainCameraTransformComponent);
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

bool CameraSystem::GetTargetMode(CameraComponent& mainCameraCameraComponent) const
{
	auto& useTarget = mainCameraCameraComponent.UseTarget;

	if (InputOutputData->GamePadState.IsConnected())
	{
		if (InputOutputData->GamePadState.IsXPressed())
			useTarget = !useTarget;
	}
	else
	{
		if (InputOutputData->KeyboardTracker.pressed.C)
			useTarget = !useTarget;
	}

	return useTarget;
}

DirectX::XMVECTOR CameraSystem::GetTranslation(const float cameraSpeed, const CameraComponent& mainCameraCameraComponent) const
{
	DirectX::XMVECTOR translationVector{};

	if (InputOutputData->GamePadState.IsConnected())
	{
		if (InputOutputData->GamePadState.dpad.up)
			translationVector = DirectX::XMVectorAdd(DirectX::XMVectorScale(mainCameraCameraComponent.VectorForward, cameraSpeed), translationVector);
		if (InputOutputData->GamePadState.dpad.left)
			translationVector = DirectX::XMVectorAdd(DirectX::XMVectorScale(mainCameraCameraComponent.VectorRight, -cameraSpeed), translationVector);
		if (InputOutputData->GamePadState.dpad.down)
			translationVector = DirectX::XMVectorAdd(DirectX::XMVectorScale(mainCameraCameraComponent.VectorForward, -cameraSpeed), translationVector);
		if (InputOutputData->GamePadState.dpad.right)
			translationVector = DirectX::XMVectorAdd(DirectX::XMVectorScale(mainCameraCameraComponent.VectorRight, cameraSpeed), translationVector);
	}
	else
	{
		if (InputOutputData->KeyboardState.W)
			translationVector = DirectX::XMVectorAdd(DirectX::XMVectorScale(mainCameraCameraComponent.VectorForward, cameraSpeed), translationVector);
		if (InputOutputData->KeyboardState.A)
			translationVector = DirectX::XMVectorAdd(DirectX::XMVectorScale(mainCameraCameraComponent.VectorRight, -cameraSpeed), translationVector);
		if (InputOutputData->KeyboardState.S)
			translationVector = DirectX::XMVectorAdd(DirectX::XMVectorScale(mainCameraCameraComponent.VectorForward, -cameraSpeed), translationVector);
		if (InputOutputData->KeyboardState.D)
			translationVector = DirectX::XMVectorAdd(DirectX::XMVectorScale(mainCameraCameraComponent.VectorRight, cameraSpeed), translationVector);
	}

	return translationVector;
}

std::pair<float, float> CameraSystem::GetRotation(const float deltaTime, const CameraComponent& mainCameraCameraComponent, 
	const TransformComponent& mainCameraTransformComponent) const
{
	auto rotationX = 0.0f;
	auto rotationY = 0.0f;
	TransformLogic::GetRotation(&rotationX, &rotationY, nullptr, mainCameraTransformComponent);

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

	if (rotationX > mainCameraCameraComponent.MaxRotationY)
		rotationX = mainCameraCameraComponent.MaxRotationY;
	else if (rotationX < mainCameraCameraComponent.MinRotationY)
		rotationX = mainCameraCameraComponent.MinRotationY;

	return std::pair<float, float>(rotationX, rotationY);
}

void CameraSystem::UpdateViewMatrix(CameraComponent& mainCameraCameraComponent, TransformComponent& mainCameraTransformComponent) const
{
	const auto position = TransformLogic::GetPosition(mainCameraTransformComponent);
	const auto positionVector = XMLoadFloat3(&position);

	const auto rotation = TransformLogic::GetRotationEuler(mainCameraTransformComponent);
	const auto rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);

	const auto cameraDirection = XMVector3TransformCoord(TransformComponent::FORWARD_VECTOR, rotationMatrix);
	const auto cameraTarget = DirectX::XMVectorAdd(cameraDirection, positionVector);
	const auto upDirection = XMVector3TransformCoord(TransformComponent::UP_VECTOR, rotationMatrix);

	mainCameraCameraComponent.ViewMatrix = DirectX::XMMatrixLookAtLH(positionVector, cameraTarget, upDirection);

	mainCameraCameraComponent.VectorForward = cameraDirection;
	mainCameraCameraComponent.VectorRight = XMVector3TransformCoord(TransformComponent::RIGHT_VECTOR, rotationMatrix);
}
