#pragma once
#include <DirectXMath.h>
#include <array>

class ShadowCamera final
{
public:
	ShadowCamera();

	void UpdateNearAndFarZ(float nearZ, float farZ);

	void UpdateLightDirection(float lightDirectionX, float lightDirectionY, float lightDirectionZ);
	void UpdateShadowMapLocation(const DirectX::XMMATRIX& cameraViewMatrix, const DirectX::XMMATRIX& cameraProjectionMatrix);

	DirectX::XMMATRIX CalculateCameraMatrix() const;
private:
	DirectX::XMVECTOR UpVector{};
	DirectX::XMVECTOR EyeVector{};

	DirectX::XMMATRIX ProjectionMatrix{};

	DirectX::XMFLOAT3 LightDirection{};
	DirectX::XMMATRIX ViewMatrix{};

	float NearZ = 0.0f;
	float FarZ = 0.0f;

	std::array<DirectX::XMVECTOR, 8> GenerateGenericPoints() const;
	void CalculateFrustumPoints(std::array<DirectX::XMVECTOR, 8>& points,
		const DirectX::XMMATRIX& viewMatrix, const DirectX::XMMATRIX& projectionMatrix) const;
	DirectX::XMMATRIX GenerateProjectionMatrix(std::array<DirectX::XMVECTOR, 8>& points) const;

	void StabilizeCamera(float* viewLeft, float* viewRight, float* viewBottom, float* viewTop, const std::array<DirectX::XMVECTOR, 8>& points) const;
};
