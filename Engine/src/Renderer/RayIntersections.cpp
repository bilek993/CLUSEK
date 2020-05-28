#include "RayIntersections.h"

bool RayIntersections::TestObb(	const DirectX::XMVECTOR& rayOrigin, 
								const DirectX::XMVECTOR& rayDirection,
								const DirectX::XMVECTOR& aabbMin, 
								const DirectX::XMVECTOR& aabbMax, 
								const DirectX::XMMATRIX& modelMatrix,
								const float maxDistance,
								float* intersectionDistance)
{
	return true; // TODO: Change this
}
