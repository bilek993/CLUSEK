#include "GrassCamera.h"

DirectX::XMMATRIX GrassCamera::GenerateCameraMatrix(const TransformComponent& mainCameraTransformComponent) // TODO: Fix this function
{
	const auto viewMatrix = DirectX::XMMatrixLookAtLH(	ZeroVector, 
														DownVector, 
														UpVector);
	
	const auto projectionMatrix = DirectX::XMMatrixOrthographicOffCenterLH(	-500.0f, 
																			500.0f, 
																			-500.0f, 
																			500.0f, 
																			-10000.0f, 
																			10000.0f);
	
	return viewMatrix * projectionMatrix;
}
