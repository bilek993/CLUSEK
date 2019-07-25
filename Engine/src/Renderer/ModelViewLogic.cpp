#include "ModelViewLogic.h"

void ModelViewLogic::UpdateViewMatrix(CameraComponent& cameraComponent, TransformComponent& transformComponent)
{
	const auto cameraRotationMatrix = DirectX::XMMatrixRotationRollPitchYawFromVector(transformComponent.RotationVector);
	const auto cameraDirection = XMVector3TransformCoord(TransformComponent::FORWARD_VECTOR, cameraRotationMatrix);
	const auto cameraTarget = DirectX::XMVectorAdd(cameraDirection, transformComponent.PositionVector);
	const auto upDirection = XMVector3TransformCoord(TransformComponent::UP_VECTOR, cameraRotationMatrix);
	cameraComponent.ViewMatrix = DirectX::XMMatrixLookAtLH(transformComponent.PositionVector, cameraTarget, upDirection);

	transformComponent.VectorForward = cameraDirection;
	transformComponent.VectorRight = XMVector3TransformCoord(TransformComponent::RIGHT_VECTOR, cameraRotationMatrix);
}

void ModelViewLogic::UpdateModelMatrix(RenderComponent& renderComponent, TransformComponent& transformComponent)
{
	const auto vecRotationMatrix = DirectX::XMMatrixRotationRollPitchYawFromVector(transformComponent.RotationVector);
	const auto vecTranslationMatrix = DirectX::XMMatrixTranslationFromVector(transformComponent.PositionVector);
	renderComponent.WorldMatrix = XMMatrixMultiply(vecRotationMatrix, vecTranslationMatrix);

	transformComponent.VectorForward = XMVector3TransformCoord(TransformComponent::FORWARD_VECTOR, vecRotationMatrix);
	transformComponent.VectorRight = XMVector3TransformCoord(TransformComponent::RIGHT_VECTOR, vecRotationMatrix);
}
