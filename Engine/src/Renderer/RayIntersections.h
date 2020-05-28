#pragma once
#include <DirectXMath.h>

class RayIntersections final
{
public:
	static bool TestObb(const DirectX::XMVECTOR& rayOrigin,
						const DirectX::XMVECTOR& rayDirection,
						const DirectX::XMVECTOR& aabbMin,
						const DirectX::XMVECTOR& aabbMax,
						const DirectX::XMMATRIX& modelMatrix,
						float maxDistance,
						float* intersectionDistance);
};
