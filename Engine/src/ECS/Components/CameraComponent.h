#pragma once
#include <DirectXMath.h>

struct CameraComponent final
{
	DirectX::XMMATRIX ViewMatrix;
	DirectX::XMMATRIX ProjectionMatrix;
	float MaxRotationY;
	float MinRotationY;
	// 0 - Left Frustum Plane
	// 1 - RightFrustum Plane
	// 2 - Top Frustum Plane
	// 3 - Bottom Frustum Plane
	// 4 - Near Frustum Plane
	// 5 - Far Frustum Plane
	DirectX::XMVECTOR FrustumPlanes[6]{};
};
