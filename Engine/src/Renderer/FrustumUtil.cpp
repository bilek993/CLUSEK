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
