#include "ModelViewLogic.h"

void ModelViewLogic::UpdateViewMatrix(CameraComponent& cameraComponent, TransformComponent& transformComponent)
{
	DirectX::XMMATRIX cameraRotationMatrix{};

	if (transformComponent.RotationModeForChanges == EulerAngels)
		cameraRotationMatrix = DirectX::XMMatrixRotationRollPitchYawFromVector(transformComponent.RotationVectorEuler);
	else
		cameraRotationMatrix = DirectX::XMMatrixRotationQuaternion(transformComponent.RotationVectorQuat);

	const auto vecTranslationMatrix = DirectX::XMMatrixTranslationFromVector(transformComponent.PositionVector);

	const auto cameraDirection = XMVector3TransformCoord(TransformComponent::FORWARD_VECTOR, cameraRotationMatrix);
	const auto cameraTarget = DirectX::XMVectorAdd(cameraDirection, transformComponent.PositionVector);
	const auto upDirection = XMVector3TransformCoord(TransformComponent::UP_VECTOR, cameraRotationMatrix);
	cameraComponent.ViewMatrix = DirectX::XMMatrixLookAtLH(transformComponent.PositionVector, cameraTarget, upDirection);

	transformComponent.VectorForward = cameraDirection;
	transformComponent.VectorRight = XMVector3TransformCoord(TransformComponent::RIGHT_VECTOR, cameraRotationMatrix);
}

void ModelViewLogic::UpdateModelMatrix(DirectX::XMMATRIX& worldMatrix, TransformComponent& transformComponent)
{
	DirectX::XMMATRIX vecRotationMatrix{};

	if (transformComponent.RotationModeForChanges == EulerAngels)
		vecRotationMatrix = DirectX::XMMatrixRotationRollPitchYawFromVector(transformComponent.RotationVectorEuler);
	else
		vecRotationMatrix = DirectX::XMMatrixRotationQuaternion(transformComponent.RotationVectorQuat);

	const auto vecTranslationMatrix = DirectX::XMMatrixTranslationFromVector(transformComponent.PositionVector);
	worldMatrix = XMMatrixMultiply(vecRotationMatrix, vecTranslationMatrix);

	transformComponent.VectorForward = XMVector3TransformCoord(TransformComponent::FORWARD_VECTOR, vecRotationMatrix);
	transformComponent.VectorRight = XMVector3TransformCoord(TransformComponent::RIGHT_VECTOR, vecRotationMatrix);
}

DirectX::XMMATRIX ModelViewLogic::GenerateOrthographicProjectionMatrix(const float ratio)
{
	return DirectX::XMMatrixOrthographicLH(ratio, 1, -1.0f, 1.0f);
}
