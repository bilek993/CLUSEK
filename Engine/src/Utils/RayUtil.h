#pragma once
#include <DirectXMath.h>
#include <array>

class RayUtil final
{
public:
	static void MousePositionToRayOriginAndDirection(	float mouseX,
														float mouseY,
														DirectX::XMMATRIX& viewMatrix,
														DirectX::XMMATRIX& projectionMatrix,
														DirectX::XMVECTOR& rayOriginOutput,
														DirectX::XMVECTOR& rayDirectionOutput);

	static bool TestObb(const DirectX::XMVECTOR& rayOrigin,
						const DirectX::XMVECTOR& rayDirection,
						const std::array<DirectX::XMFLOAT3, 8>& aabbPoints,
						const DirectX::XMMATRIX& worldMatrix,
						float maxDistance,
						float* intersectionDistance);

	static bool TestObb(const DirectX::XMVECTOR& rayOrigin,
						const DirectX::XMVECTOR& rayDirection,
						const DirectX::XMVECTOR& aabbMin,
						const DirectX::XMVECTOR& aabbMax,
						const DirectX::XMMATRIX& worldMatrix,
						float maxDistance,
						float* intersectionDistance);

private:
	static bool TestObbIntersectionWithPlanes(	const DirectX::XMVECTOR& axis,
												const DirectX::XMVECTOR& delta,
												const DirectX::XMVECTOR& rayDirection,
												float aabbMinOnAxis,
												float aabbMaxOnAxis,
												float& tMax,
												float& tMin);
};
