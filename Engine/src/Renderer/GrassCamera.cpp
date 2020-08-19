#include "GrassCamera.h"

#include "TransformLogic.h"

DirectX::XMFLOAT3 GrassCamera::AlignCameraToGrid(const DirectX::XMFLOAT3& cameraPosition, const float width, const float height, const int resolution)
{
	// 'gridSizeX' and 'gridSizeY' multiplied by 4 due to LOD
	const auto gridSizeX = (width * 4) / static_cast<float>(resolution);
	const auto gridSizeY = (height * 4) / static_cast<float>(resolution);
	
	return DirectX::XMFLOAT3(roundf(cameraPosition.x / gridSizeX) * gridSizeX, cameraPosition.y, roundf(cameraPosition.z / gridSizeY) * gridSizeY);
}

DirectX::XMMATRIX GrassCamera::GenerateCameraMatrix(const TransformComponent& mainCameraTransformComponent, 
                                                    const float distance, 
                                                    const float width, 
                                                    const float height, 
                                                    const int resolution)
{
	const auto cameraPosition = AlignCameraToGrid(	TransformLogic::GetPosition(mainCameraTransformComponent), 
													width, 
													height, 
													resolution);
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
