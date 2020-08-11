#pragma once
#include <DirectXMath.h>
#include "../ECS/Components/TransformComponent.h"

class GrassCamera final
{
private:
	inline static DirectX::XMVECTOR UpVector = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	inline static DirectX::XMVECTOR DownVector = DirectX::XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f);

	static DirectX::XMFLOAT3 AlignCameraToGrid(const DirectX::XMFLOAT3& cameraPosition, float width, float height, int resolution);
	
public:
	static DirectX::XMMATRIX GenerateCameraMatrix(const TransformComponent& mainCameraTransformComponent, float distance, float width, float height, int resolution);
};
