#include "ShadowCamera.h"
#include <cmath>

ShadowCamera::ShadowCamera()
{
	DirectX::XMFLOAT3 upFloats(0, 1, 0);
	UpVector = XMLoadFloat3(&upFloats);

	DirectX::XMFLOAT3 eyeFloats(0, 0, 0);
	EyeVector = XMLoadFloat3(&eyeFloats);
}

void ShadowCamera::UpdateLightDirection(const float lightDirectionX, const float lightDirectionY, const float lightDirectionZ)
{
	if (std::fabs(LightDirection.x - lightDirectionX) < 0.01f &&
		std::fabs(LightDirection.y - lightDirectionY) < 0.01f &&
		std::fabs(LightDirection.z - lightDirectionZ) < 0.01f)
		return;

	LightDirection.x = lightDirectionX;
	LightDirection.y = lightDirectionY;
	LightDirection.z = lightDirectionZ;

	const auto lightDirectionVector = XMLoadFloat3(&LightDirection);
	ViewMatrix = DirectX::XMMatrixLookAtLH(EyeVector, lightDirectionVector, UpVector);
}

void ShadowCamera::UpdateShadowMapLocation(const DirectX::XMMATRIX& cameraViewMatrix, const DirectX::XMMATRIX& cameraProjectionMatrix)
{
	auto frustumPoints = GenerateGenericPoints();
	CalculateFrustumPoints(frustumPoints, cameraViewMatrix, cameraProjectionMatrix);

	ProjectionMatrix = GenerateProjectionMatrix(frustumPoints);
}

DirectX::XMMATRIX ShadowCamera::CalculateCameraMatrix() const
{
	return ViewMatrix * ProjectionMatrix;
}

std::array<DirectX::XMVECTOR, 8> ShadowCamera::GenerateGenericPoints() const
{
	const std::array<DirectX::XMVECTOR, 8> vectorPoints
	{
		DirectX::XMVectorSet(1.0f, 1.0f, -1.0f, 1.0f),
		DirectX::XMVectorSet(-1.0f, 1.0f, -1.0f, 1.0f),
		DirectX::XMVectorSet(1.0f, -1.0f, -1.0f, 1.0f),
		DirectX::XMVectorSet(-1.0f, -1.0f, -1.0f, 1.0f),
		DirectX::XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f),
		DirectX::XMVectorSet(-1.0f, 1.0f, 1.0f, 1.0f),
		DirectX::XMVectorSet(1.0f, -1.0f, 1.0f, 1.0f),
		DirectX::XMVectorSet(-1.0f, -1.0f, 1.0f, 1.0f),
	};
	return vectorPoints;
}

void ShadowCamera::CalculateFrustumPoints(std::array<DirectX::XMVECTOR, 8>& points,
	const DirectX::XMMATRIX& viewMatrix, const DirectX::XMMATRIX& projectionMatrix) const
{
	const auto invertedCameraMatrix = XMMatrixInverse(nullptr, (projectionMatrix * viewMatrix));

	for (auto& point : points)
	{
		point = XMVector4Transform(point, invertedCameraMatrix);

		DirectX::XMFLOAT4 pointValues{};
		XMStoreFloat4(&pointValues, point);

		const auto scale = 1.0f / pointValues.w;

		pointValues.x *= scale;
		pointValues.y *= scale;
		pointValues.z *= scale;
		pointValues.w = 1.0f;

		point = XMLoadFloat4(&pointValues);
	}
}

DirectX::XMMATRIX ShadowCamera::GenerateProjectionMatrix(std::array<DirectX::XMVECTOR, 8>& points) const
{
	return DirectX::XMMatrixOrthographicOffCenterLH(-100.0f, 100.0f, -100.0f, 100.0f, -100.0f, 100.0f);
}
