#pragma once
#include <DirectXMath.h>
#include <array>

class FrustumUtil final
{
public:
	static std::array<DirectX::XMVECTOR, 6> CalculateFrustumPlanes(const DirectX::XMFLOAT4X4& viewProjectionMatrix);
	static std::array<DirectX::XMVECTOR, 6> CalculateFrustumPlanes(const DirectX::XMMATRIX& viewProjectionMatrix);
};
