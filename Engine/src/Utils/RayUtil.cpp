#include "RayUtil.h"
#include "algorithm"

void RayUtil::MousePositionToRayOriginAndDirection(	float mouseX, 
													float mouseY, 
													DirectX::XMMATRIX& viewMatrix,
													DirectX::XMMATRIX& projectionMatrix, 
													DirectX::XMVECTOR& rayOriginOutput, 
													DirectX::XMVECTOR& rayDirectionOutput)
{
	mouseX = (mouseX * 2.0f) - 1.0f;
	mouseY = (mouseY * 2.0f) - 1.0f;

	const auto rayStartNormalizedDeviceCoordinates = DirectX::XMFLOAT4(mouseX, mouseY, -1.0f, 1.0f);
	const auto rayEndNormalizedDeviceCoordinates = DirectX::XMFLOAT4(mouseX, mouseY, 0.0f, 1.0f);

	const auto rayStartNormalizedDeviceCoordinatesVector = XMLoadFloat4(&rayStartNormalizedDeviceCoordinates);
	const auto rayEndNormalizedDeviceCoordinatesVector = XMLoadFloat4(&rayEndNormalizedDeviceCoordinates);

	// TODO: Optimize this
	const auto projectionMatrixInverse = XMMatrixInverse(nullptr, projectionMatrix);
	const auto viewMatrixInverse = XMMatrixInverse(nullptr, viewMatrix);

	auto rayStartCamera = XMVector4Transform(rayStartNormalizedDeviceCoordinatesVector, projectionMatrixInverse);
	rayStartCamera = DirectX::XMVectorScale(rayStartCamera, 1.0f / DirectX::XMVectorGetW(rayStartCamera));

	auto rayStartWorld = XMVector4Transform(rayStartCamera, viewMatrixInverse);
	rayStartWorld = DirectX::XMVectorScale(rayStartWorld, 1.0f / DirectX::XMVectorGetW(rayStartCamera));

	auto rayEndCamera = XMVector4Transform(rayEndNormalizedDeviceCoordinatesVector, projectionMatrixInverse);
	rayEndCamera = DirectX::XMVectorScale(rayEndCamera, 1.0f / DirectX::XMVectorGetW(rayEndCamera));

	auto rayEndWorld = XMVector4Transform(rayEndCamera, viewMatrixInverse);
	rayEndWorld = DirectX::XMVectorScale(rayEndWorld, 1.0f / DirectX::XMVectorGetW(rayEndCamera));

	const auto rayDir = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(rayEndWorld, rayStartWorld));

	rayOriginOutput = rayStartWorld;
	rayDirectionOutput = rayDir;
}

bool RayUtil::TestObb(	const DirectX::XMVECTOR& rayOrigin,
						const DirectX::XMVECTOR& rayDirection,
						const std::array<DirectX::XMFLOAT3, 8>& aabbPoints, 
						const DirectX::XMMATRIX& worldMatrix,
						const float maxDistance,
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
					worldMatrix,
					maxDistance,
					intersectionDistance);
}

bool RayUtil::TestObb(	const DirectX::XMVECTOR& rayOrigin,
						const DirectX::XMVECTOR& rayDirection,
						const DirectX::XMVECTOR& aabbMin, 
						const DirectX::XMVECTOR& aabbMax, 
						const DirectX::XMMATRIX& worldMatrix,
						const float maxDistance,
						float* intersectionDistance)
{
	auto tMin = 0.0f;
	auto tMax = maxDistance;

	DirectX::XMFLOAT4X4 worldMatrixFloats{};
	XMStoreFloat4x4(&worldMatrixFloats, worldMatrix);

	DirectX::XMFLOAT3 aabbMinFloats{};
	DirectX::XMFLOAT3 aabbMaxFloats{};

	XMStoreFloat3(&aabbMinFloats, aabbMin);
	XMStoreFloat3(&aabbMaxFloats, aabbMax);

	const auto worldPosition = DirectX::XMFLOAT3(worldMatrixFloats._41, worldMatrixFloats._42, worldMatrixFloats._43);
	const auto worldPositionVector = XMLoadFloat3(&worldPosition);

	const auto deltaPositionVector = DirectX::XMVectorSubtract(worldPositionVector, rayOrigin);

	const auto xAxis = DirectX::XMFLOAT3(worldMatrixFloats._11, worldMatrixFloats._12, worldMatrixFloats._13);
	const auto xAxisVector = XMLoadFloat3(&xAxis);

	if (!TestObbIntersectionWithPlanes(xAxisVector, deltaPositionVector, rayDirection, aabbMinFloats.x, aabbMaxFloats.x, tMax, tMin))
		return false;

	const auto yAxis = DirectX::XMFLOAT3(worldMatrixFloats._21, worldMatrixFloats._22, worldMatrixFloats._23);
	const auto yAxisVector = XMLoadFloat3(&yAxis);

	if (!TestObbIntersectionWithPlanes(yAxisVector, deltaPositionVector, rayDirection, aabbMinFloats.y, aabbMaxFloats.y, tMax, tMin))
		return false;

	const auto zAxis = DirectX::XMFLOAT3(worldMatrixFloats._31, worldMatrixFloats._32, worldMatrixFloats._33);
	const auto zAxisVector = XMLoadFloat3(&zAxis);

	if (!TestObbIntersectionWithPlanes(zAxisVector, deltaPositionVector, rayDirection, aabbMinFloats.z, aabbMaxFloats.z, tMax, tMin))
		return false;

	if (intersectionDistance != nullptr)
		*intersectionDistance = tMin;

	return true;
}

bool RayUtil::TestObbIntersectionWithPlanes(	const DirectX::XMVECTOR& axis,
												const DirectX::XMVECTOR& delta,
												const DirectX::XMVECTOR& rayDirection, 
												const float aabbMinOnAxis,
												const float aabbMaxOnAxis,
												float& tMax,
												float& tMin)
{
	const auto e = DirectX::XMVectorGetX(DirectX::XMVector3Dot(axis, delta));
	const auto f = DirectX::XMVectorGetX(DirectX::XMVector3Dot(rayDirection, axis));

	if (fabsf(f) > 0.001f)
	{
		auto t1 = (e + aabbMinOnAxis) / f;
		auto t2 = (e + aabbMaxOnAxis) / f;

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
		if (-e + aabbMinOnAxis > 0.0f || -e + aabbMaxOnAxis < 0.0f)
			return false;
	}

	return true;
}
