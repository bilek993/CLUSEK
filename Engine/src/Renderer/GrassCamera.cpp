#include "GrassCamera.h"

#include "TransformLogic.h"

DirectX::XMMATRIX GrassCamera::GenerateCameraMatrix(const TransformComponent& mainCameraTransformComponent)
{
	const auto cameraPosition = TransformLogic::GetPosition(mainCameraTransformComponent);
	const auto cameraPositionVector = XMLoadFloat3(&cameraPosition);
	
	const auto viewMatrix = DirectX::XMMatrixLookAtLH(	cameraPositionVector,
														DirectX::XMVectorAdd(cameraPositionVector, DownVector), 
														UpVector);
	
	const auto projectionMatrix = DirectX::XMMatrixOrthographicOffCenterLH(	-500.0f, 
																			500.0f, 
																			-500.0f, 
																			500.0f, 
																			-10000.0f, 
																			10000.0f);
	
	return viewMatrix * projectionMatrix;
}
