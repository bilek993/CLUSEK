#pragma once
#include <DirectXMath.h>
#include <array>
#include "Vertex/FatVertex.h"
#include <vector>

class FrustumUtil final
{
public:
	// 0 - Left Frustum Plane
	// 1 - RightFrustum Plane
	// 2 - Top Frustum Plane
	// 3 - Bottom Frustum Plane
	// 4 - Near Frustum Plane
	// 5 - Far Frustum Plane
	static std::array<DirectX::XMVECTOR, 6> CalculateFrustumPlanes(const DirectX::XMFLOAT4X4& viewProjectionMatrix);
	static std::array<DirectX::XMVECTOR, 6> CalculateFrustumPlanes(const DirectX::XMMATRIX& viewProjectionMatrix);

	static std::array<DirectX::XMFLOAT3, 8> CalculateAABB(const std::vector<FatVertex>& vertices);
	static std::array<DirectX::XMFLOAT3, 8> RecalculateAABBForWorld(const std::array<DirectX::XMFLOAT3, 8>& points, 
		const DirectX::XMMATRIX& worldMatrix);

	static bool Test(const std::array<DirectX::XMFLOAT3, 8>& points, const std::array<DirectX::XMVECTOR, 6>& cameraFrustumPlanes);
};
