#include "ModelViewLogic.h"
#include "TransformLogic.h"

void ModelViewLogic::UpdateViewMatrix(CameraComponent& cameraComponent, TransformComponent& transformComponent)
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

DirectX::XMMATRIX ModelViewLogic::GenerateOrthographicProjectionMatrix(const float ratio)
{
	return DirectX::XMMatrixOrthographicLH(ratio, 1, -1.0f, 1.0f);
}
