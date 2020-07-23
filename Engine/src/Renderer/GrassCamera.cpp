#include "GrassCamera.h"

#include "TransformLogic.h"

DirectX::XMMATRIX GrassCamera::GenerateCameraMatrix(const TransformComponent& mainCameraTransformComponent, const float distance, const float width, const float height)
{
	const auto cameraPosition = TransformLogic::GetPosition(mainCameraTransformComponent);
	const auto cameraPositionVector = XMLoadFloat3(&cameraPosition);
	
	const auto viewMatrix = DirectX::XMMatrixLookAtLH(	cameraPositionVector,
														DirectX::XMVectorAdd(cameraPositionVector, DownVector), 
														UpVector);
	
	const auto projectionMatrix = DirectX::XMMatrixOrthographicOffCenterLH(	-width / 2,
																			width / 2,
																			-height / 2,
																			height / 2,
																			-1.0f,
																			distance);
	
	return viewMatrix * projectionMatrix;
}
