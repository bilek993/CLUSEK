#include "RayIntersections.h"
#include "algorithm"

bool RayIntersections::TestObb(	const DirectX::XMVECTOR& rayOrigin, 
								const DirectX::XMVECTOR& rayDirection,
								const std::array<DirectX::XMFLOAT3, 8>& aabbPoints, 
								const DirectX::XMMATRIX& modelMatrix, 
								float maxDistance,
								float* intersectionDistance)
{
	auto aabbMin = DirectX::XMFLOAT3(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
	auto aabbMax = DirectX::XMFLOAT3(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());

	for (auto& point : aabbPoints)
	{
		if (point.x < aabbMin.x)
			aabbMin.x = point.x;
		if (point.y < aabbMin.y)
			aabbMin.y = point.y;
		if (point.z < aabbMin.z)
			aabbMin.z = point.z;

		if (point.x > aabbMax.x)
			aabbMax.x = point.x;
		if (point.y > aabbMax.y)
			aabbMax.y = point.y;
		if (point.z > aabbMax.z)
			aabbMax.z = point.z;
	}

	return TestObb(	rayOrigin,
					rayDirection,
					XMLoadFloat3(&aabbMin),
					XMLoadFloat3(&aabbMax),
					modelMatrix,
					maxDistance,
					intersectionDistance);
}

bool RayIntersections::TestObb(	const DirectX::XMVECTOR& rayOrigin, 
								const DirectX::XMVECTOR& rayDirection,
								const DirectX::XMVECTOR& aabbMin, 
								const DirectX::XMVECTOR& aabbMax, 
								const DirectX::XMMATRIX& modelMatrix,
								const float maxDistance,
								float* intersectionDistance)
{
	auto tMin = 0.0f;
	auto tMax = maxDistance;

	DirectX::XMFLOAT4X4 modelMatrixFloats{};
	XMStoreFloat4x4(&modelMatrixFloats, modelMatrix);

	const auto worldPosition = DirectX::XMFLOAT3(modelMatrixFloats._41, modelMatrixFloats._42, modelMatrixFloats._43);
	const auto worldPositionVector = XMLoadFloat3(&worldPosition);

	const auto deltaPositionVector = DirectX::XMVectorSubtract(worldPositionVector, rayOrigin);

	const auto xAxis = DirectX::XMFLOAT3(modelMatrixFloats._11, modelMatrixFloats._12, modelMatrixFloats._13);
	const auto xAxisVector = XMLoadFloat3(&xAxis);

	if (!TestObbIntersectionWithPlanes(xAxisVector, deltaPositionVector, rayDirection, aabbMin, aabbMax, tMax, tMin))
		return false;

	const auto yAxis = DirectX::XMFLOAT3(modelMatrixFloats._21, modelMatrixFloats._22, modelMatrixFloats._23);
	const auto yAxisVector = DirectX::XMLoadFloat3(&yAxis);

	if (!TestObbIntersectionWithPlanes(yAxisVector, deltaPositionVector, rayDirection, aabbMin, aabbMax, tMax, tMin))
		return false;

	const auto zAxis = DirectX::XMFLOAT3(modelMatrixFloats._31, modelMatrixFloats._32, modelMatrixFloats._33);
	const auto zAxisVector = XMLoadFloat3(&zAxis);

	if (!TestObbIntersectionWithPlanes(zAxisVector, deltaPositionVector, rayDirection, aabbMin, aabbMax, tMax, tMin))
		return false;

	if (intersectionDistance != nullptr)
		*intersectionDistance = tMin;

	return true;
}

bool RayIntersections::TestObbIntersectionWithPlanes(	const DirectX::XMVECTOR& axis, 
														const DirectX::XMVECTOR& delta,
														const DirectX::XMVECTOR& rayDirection, 
														const DirectX::XMVECTOR& aabbMin, 
														const DirectX::XMVECTOR& aabbMax,
														float& tMax,
														float& tMin)
{
	DirectX::XMFLOAT3 aabbMinFloats{};
	DirectX::XMFLOAT3 aabbMaxFloats{};

	XMStoreFloat3(&aabbMinFloats, aabbMin);
	XMStoreFloat3(&aabbMaxFloats, aabbMax);

	const auto e = DirectX::XMVectorGetX(DirectX::XMVector3Dot(axis, delta));
	const auto f = DirectX::XMVectorGetX(DirectX::XMVector3Dot(axis, delta));

	if (fabsf(f) > 0.001f)
	{
		auto t1 = (e + aabbMinFloats.x) / f;
		auto t2 = (e + aabbMaxFloats.x) / f;

		if (t1 > t2)
			std::swap(t1, t2);

		if (t2 < tMax)
			tMax = t2;

		if (t1 > tMin)
			tMin = t1;

		if (tMax < tMin)
			return false;
	}
	else
	{
		if (-e + aabbMinFloats.x > 0.0f || -e + aabbMaxFloats.x < 0.0f)
			return false;
	}

	return true;
}
