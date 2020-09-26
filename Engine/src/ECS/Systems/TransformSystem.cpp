#include "TransformSystem.h"
#include "../Components/TransformComponent.h"
#include "../../Renderer/TransformLogic.h"
#include "../Components/CameraTargetComponent.h"

void TransformSystem::Start()
{
	Logger::Debug("Staring transform system...");

	Registry->view<TransformComponent>().each([](TransformComponent &transformComponent)
	{
		TransformLogic::SetPositionAndRotationEuler(	transformComponent.InitialPositionVector,
														transformComponent.InitialRotationVector, 
														transformComponent);
	});

	Registry->view<CameraTargetComponent>().each([](CameraTargetComponent &cameraTargetComponent)
	{
		const auto translationMatrix = DirectX::XMMatrixTranslationFromVector(XMLoadFloat3(&cameraTargetComponent.InitialPositionVector));
		const auto rotationMatrix = DirectX::XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&cameraTargetComponent.InitialRotationVector));

		cameraTargetComponent.PaddingMatrix = rotationMatrix * translationMatrix;
	});
}

void TransformSystem::Rebuild()
{
}

void TransformSystem::Update(float deltaTime)
{
}
