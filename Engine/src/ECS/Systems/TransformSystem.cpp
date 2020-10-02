#include "TransformSystem.h"
#include "../../Tags.h"
#include "../Components/TransformComponent.h"
#include "../../Renderer/TransformLogic.h"
#include "../Components/CameraComponent.h"
#include "../Components/CameraTargetComponent.h"

void TransformSystem::Start()
{
}

void TransformSystem::Rebuild()
{
	Logger::Debug("Rebuilding transform system...");

	Registry->view<TransformComponent, entt::tag<Tags::REQUIRES_REBUILD>>().each([](TransformComponent &transformComponent, auto _)
	{
		TransformLogic::SetPositionAndRotationEuler(transformComponent.InitialPositionVector,
													transformComponent.InitialRotationVector,
													transformComponent);
	});

	Registry->view<TransformComponent, CameraComponent, entt::tag<Tags::REQUIRES_REBUILD>>().each([](TransformComponent &transformComponent, CameraComponent &cameraComponent, auto _)
	{
		transformComponent.InitialRotationVector.x *= M_PI / 180.0f;
		transformComponent.InitialRotationVector.y *= M_PI / 180.0f;
		transformComponent.InitialRotationVector.z *= M_PI / 180.0f;
		
		TransformLogic::SetPositionAndRotationEuler(transformComponent.InitialPositionVector,
													transformComponent.InitialRotationVector,
													transformComponent);
	});

	Registry->view<CameraTargetComponent, entt::tag<Tags::REQUIRES_REBUILD>>().each([](CameraTargetComponent &cameraTargetComponent, auto _)
	{
		const auto translationMatrix = DirectX::XMMatrixTranslationFromVector(XMLoadFloat3(&cameraTargetComponent.InitialPositionVector));
		const auto rotationMatrix = DirectX::XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&cameraTargetComponent.InitialRotationVector));

		cameraTargetComponent.PaddingMatrix = rotationMatrix * translationMatrix;
	});
}

void TransformSystem::Update(float deltaTime)
{
}
