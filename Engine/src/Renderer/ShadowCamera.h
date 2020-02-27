#pragma once
#include <DirectXMath.h>
#include <array>

class ShadowCamera final
{
public:
	ShadowCamera();

	inline static const int SHADOW_CASCADES_COUNT = 4;

	void UpdateShadowResolution(int resolution);
	void UpdateNearZShift(float shift);
	void UpdateMainCameraProjectionMatrix(	int level, 
											int mainCameraFov,
											int windowWidth,
											int windowHeight,
											float mainCameraNearZ,
											const std::array<float, SHADOW_CASCADES_COUNT>& cascadeEnds);

	void UpdateLightDirection(float lightDirectionX, float lightDirectionY, float lightDirectionZ);
	void UpdateShadowMapLocation(const DirectX::XMMATRIX& cameraViewMatrix);

	DirectX::XMMATRIX CalculateCameraMatrix() const;

	std::array<DirectX::XMVECTOR, 6>& GetFrustumPlanes();
private:
	DirectX::XMVECTOR UpVector{};
	DirectX::XMVECTOR EyeVector{};

	DirectX::XMMATRIX MainCameraProjectionMatrix{};

	DirectX::XMMATRIX ProjectionMatrix{};

	DirectX::XMFLOAT3 LightDirection{};
	DirectX::XMMATRIX ViewMatrix{};

	std::array<DirectX::XMVECTOR, 6> FrustumPlanes{};

	int ShadowResolution = 1024;

	float ShiftNearZ = 0.0f;

	inline static const DirectX::XMVECTOR LONGEST_DIAGONAL_CALCULATION_EPSILON = DirectX::XMVectorSet(4.0f, 4.0f, 4.0f, 4.0f);

	DirectX::XMVECTOR LongestDiagonalVector{};
	float LongestDiagonal = 0.0f;

	std::array<DirectX::XMVECTOR, 8> GenerateGenericPoints() const;
	void CalculateFrustumPoints(std::array<DirectX::XMVECTOR, 8>& points, const DirectX::XMMATRIX& viewMatrix) const;
	DirectX::XMMATRIX GenerateProjectionMatrix(std::array<DirectX::XMVECTOR, 8>& points);

	void StabilizeCamera(float* viewLeft, float* viewRight, float* viewBottom, float* viewTop, const std::array<DirectX::XMVECTOR, 8>& points);
};
