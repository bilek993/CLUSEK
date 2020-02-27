#pragma once
#include <DirectXMath.h>
#include <array>

// 0 - Left Frustum Plane
// 1 - RightFrustum Plane
// 2 - Top Frustum Plane
// 3 - Bottom Frustum Plane
// 4 - Near Frustum Plane
// 5 - Far Frustum Plane
class FrustumUtil final
{
public:
	static std::array<DirectX::XMVECTOR, 6> CalculateFrustumPlanes(const DirectX::XMFLOAT4X4& viewProjectionMatrix);
	static std::array<DirectX::XMVECTOR, 6> CalculateFrustumPlanes(const DirectX::XMMATRIX& viewProjectionMatrix);
};
