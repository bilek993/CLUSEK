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

void ModelViewLogic::UpdateModelMatrix(DirectX::XMMATRIX& worldMatrix, TransformComponent& transformComponent)
{
	const auto vecRotationMatrix = DirectX::XMMatrixRotationRollPitchYawFromVector(transformComponent.RotationVector);
	const auto vecTranslationMatrix = DirectX::XMMatrixTranslationFromVector(transformComponent.PositionVector);
	worldMatrix = XMMatrixMultiply(vecRotationMatrix, vecTranslationMatrix);

	transformComponent.VectorForward = XMVector3TransformCoord(TransformComponent::FORWARD_VECTOR, vecRotationMatrix);
	transformComponent.VectorRight = XMVector3TransformCoord(TransformComponent::RIGHT_VECTOR, vecRotationMatrix);
}

DirectX::XMMATRIX ModelViewLogic::GenerateOrthographicProjectionMatrix(const float ratio)
{
	return DirectX::XMMatrixOrthographicOffCenterLH(-1, 1, -1/ratio, 1/ratio, -1.0f, 1.0f);
}
