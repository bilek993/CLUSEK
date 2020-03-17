#include "FrustumUtil.h"

std::array<DirectX::XMVECTOR, 6> FrustumUtil::CalculateFrustumPlanes(const DirectX::XMFLOAT4X4& viewProjectionMatrix)
{
	std::array<DirectX::XMVECTOR, 6> frustumPlanes{};

	frustumPlanes[0] = DirectX::XMVectorSet(viewProjectionMatrix._14 + viewProjectionMatrix._11,
											viewProjectionMatrix._24 + viewProjectionMatrix._21,
											viewProjectionMatrix._34 + viewProjectionMatrix._31,
											viewProjectionMatrix._44 + viewProjectionMatrix._41);

	frustumPlanes[1] = DirectX::XMVectorSet(viewProjectionMatrix._14 - viewProjectionMatrix._11,
											viewProjectionMatrix._24 - viewProjectionMatrix._21,
											viewProjectionMatrix._34 - viewProjectionMatrix._31,
											viewProjectionMatrix._44 - viewProjectionMatrix._41);

	frustumPlanes[2] = DirectX::XMVectorSet(viewProjectionMatrix._14 - viewProjectionMatrix._12,
											viewProjectionMatrix._24 - viewProjectionMatrix._22,
											viewProjectionMatrix._34 - viewProjectionMatrix._32,
											viewProjectionMatrix._44 - viewProjectionMatrix._42);

	frustumPlanes[3] = DirectX::XMVectorSet(viewProjectionMatrix._14 + viewProjectionMatrix._12,
											viewProjectionMatrix._24 + viewProjectionMatrix._22,
											viewProjectionMatrix._34 + viewProjectionMatrix._32,
											viewProjectionMatrix._44 + viewProjectionMatrix._42);

	frustumPlanes[4] = DirectX::XMVectorSet(viewProjectionMatrix._13,
											viewProjectionMatrix._23,
											viewProjectionMatrix._33,
											viewProjectionMatrix._43);

	frustumPlanes[5] = DirectX::XMVectorSet(viewProjectionMatrix._14 - viewProjectionMatrix._13,
											viewProjectionMatrix._24 - viewProjectionMatrix._23,
											viewProjectionMatrix._34 - viewProjectionMatrix._33,
											viewProjectionMatrix._44 - viewProjectionMatrix._43);

	for (auto& currentPlane : frustumPlanes)
		currentPlane = DirectX::XMVector4Normalize(currentPlane);

	return frustumPlanes;
}

std::array<DirectX::XMVECTOR, 6> FrustumUtil::CalculateFrustumPlanes(const DirectX::XMMATRIX& viewProjectionMatrix)
{
	DirectX::XMFLOAT4X4 viewProjectionMatrixFloats{};
	XMStoreFloat4x4(&viewProjectionMatrixFloats, viewProjectionMatrix);

	return CalculateFrustumPlanes(viewProjectionMatrixFloats);
}

std::array<DirectX::XMFLOAT3, 8> FrustumUtil::CalculateAABB(const std::vector<FatVertex>& vertices)
{
	auto minX = std::numeric_limits<float>::max();
	auto maxX = -std::numeric_limits<float>::max();
	auto minY = std::numeric_limits<float>::max();
	auto maxY = -std::numeric_limits<float>::max();
	auto minZ = std::numeric_limits<float>::max();
	auto maxZ = -std::numeric_limits<float>::max();

	for (auto& vertex : vertices)
	{
		if (vertex.Position.x < minX)
			minX = vertex.Position.x;
		if (vertex.Position.x > maxX)
			maxX = vertex.Position.x;
		if (vertex.Position.y < minY)
			minY = vertex.Position.y;
		if (vertex.Position.y > maxY)
			maxY = vertex.Position.y;
		if (vertex.Position.z < minZ)
			minZ = vertex.Position.z;
		if (vertex.Position.z > maxZ)
			maxZ = vertex.Position.z;
	}

	const std::array<DirectX::XMFLOAT3, 8> points = {
		DirectX::XMFLOAT3(minX, minY, minZ),
		DirectX::XMFLOAT3(minX, minY, maxZ),
		DirectX::XMFLOAT3(minX, maxY, minZ),
		DirectX::XMFLOAT3(minX, maxY, maxZ),
		DirectX::XMFLOAT3(maxX, minY, minZ),
		DirectX::XMFLOAT3(maxX, minY, maxZ),
		DirectX::XMFLOAT3(maxX, maxY, minZ),
		DirectX::XMFLOAT3(maxX, maxY, maxZ),
	};

	return points;
}

std::array<DirectX::XMFLOAT3, 8> FrustumUtil::RecalculateAABBForWorld(const std::array<DirectX::XMFLOAT3, 8>& points,
	const DirectX::XMMATRIX& worldMatrix)
{
	auto minX = std::numeric_limits<float>::max();
	auto maxX = -std::numeric_limits<float>::max();
	auto minY = std::numeric_limits<float>::max();
	auto maxY = -std::numeric_limits<float>::max();
	auto minZ = std::numeric_limits<float>::max();
	auto maxZ = -std::numeric_limits<float>::max();

	for (auto point : points)
	{
		auto vector = XMLoadFloat3(&point);
		vector = XMVector3Transform(vector, worldMatrix);
		XMStoreFloat3(&point, vector);

		if (point.x < minX)
			minX = point.x;
		if (point.x > maxX)
			maxX = point.x;
		if (point.y < minY)
			minY = point.y;
		if (point.y > maxY)
			maxY = point.y;
		if (point.z < minZ)
			minZ = point.z;
		if (point.z > maxZ)
			maxZ = point.z;
	}

	const std::array<DirectX::XMFLOAT3, 8> newPoints = {
		DirectX::XMFLOAT3(minX, minY, minZ),
		DirectX::XMFLOAT3(minX, minY, maxZ),
		DirectX::XMFLOAT3(minX, maxY, minZ),
		DirectX::XMFLOAT3(minX, maxY, maxZ),
		DirectX::XMFLOAT3(maxX, minY, minZ),
		DirectX::XMFLOAT3(maxX, minY, maxZ),
		DirectX::XMFLOAT3(maxX, maxY, minZ),
		DirectX::XMFLOAT3(maxX, maxY, maxZ),
	};

	return newPoints;
}

bool FrustumUtil::Test(const std::array<DirectX::XMFLOAT3, 8>& points, const std::array<DirectX::XMVECTOR, 6>& cameraFrustumPlanes)
{
	for (auto& plane : cameraFrustumPlanes)
	{
		auto isAnyPointInside = false;

		for (auto& point : points)
		{
			auto pointFloat4 = DirectX::XMFLOAT4(point.x, point.y, point.z, 1.0f);
			const auto pointVector = XMLoadFloat4(&pointFloat4);

			if (DirectX::XMVectorGetX(DirectX::XMVector4Dot(pointVector, plane)) > 0.0f)
			{
				isAnyPointInside = true;
				break;
			}
		}

		if (!isAnyPointInside)
			return false;
	}

	return true;
}
