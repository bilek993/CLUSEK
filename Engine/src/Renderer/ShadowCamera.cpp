#include "ShadowCamera.h"
#include <cmath>
#include "../Utils/Logger.h"

ShadowCamera::ShadowCamera()
{
	DirectX::XMFLOAT3 upFloats(0, 1, 0);
	UpVector = XMLoadFloat3(&upFloats);

	DirectX::XMFLOAT3 eyeFloats(0, 0, 0);
	EyeVector = XMLoadFloat3(&eyeFloats);
}

void ShadowCamera::UpdateShadowResolution(const int resolution)
{
	ShadowResolution = resolution;
}

void ShadowCamera::UpdateNearAndFarZ(const float nearZ, const float farZ)
{
	NearZ = nearZ;
	FarZ = farZ;
}

void ShadowCamera::UpdateMainCameraProjectionMatrix(const int level, 
													const int mainCameraFov,
													const int windowWidth,
													const int windowHeight,
													const float mainCameraNearZ,
													const std::array<float, 4>& cascadeEnds)
{
	if (level > 4)
		Logger::Error("Cascade level out of range!");

	const auto fovRadians = (mainCameraFov / 360.0f) * DirectX::XM_2PI;
	const auto nearZ = level == 0 ? mainCameraNearZ : cascadeEnds[level - 1];

	MainCameraProjectionMatrix = DirectX::XMMatrixPerspectiveFovLH(fovRadians, 
		static_cast<float>(windowWidth) / static_cast<float>(windowHeight), nearZ, cascadeEnds[level]);
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

void ShadowCamera::UpdateShadowMapLocation(const DirectX::XMMATRIX& cameraViewMatrix)
{
	auto frustumPoints = GenerateGenericPoints();
	CalculateFrustumPoints(frustumPoints, cameraViewMatrix, MainCameraProjectionMatrix);

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
	const auto calculatedMatrix = XMMatrixInverse(nullptr, viewMatrix * projectionMatrix) * ViewMatrix;

	for (auto& point : points)
	{
		point = XMVector4Transform(point, calculatedMatrix);

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

DirectX::XMMATRIX ShadowCamera::GenerateProjectionMatrix(std::array<DirectX::XMVECTOR, 8>& points)
{
	auto viewLeft = FLT_MAX;
	auto viewRight = -FLT_MAX;
	auto viewBottom = FLT_MAX;
	auto viewTop = -FLT_MAX;

	for (auto& point : points)
	{
		DirectX::XMFLOAT4 pointValues{};
		XMStoreFloat4(&pointValues, point);

		if (pointValues.x < viewLeft)
			viewLeft = pointValues.x;
		if (pointValues.x > viewRight)
			viewRight = pointValues.x;
		if (pointValues.y < viewBottom)
			viewBottom = pointValues.y;
		if (pointValues.y > viewTop)
			viewTop = pointValues.y;
	}

	StabilizeCamera(&viewLeft, &viewRight, &viewBottom, &viewTop, points);

	return DirectX::XMMatrixOrthographicOffCenterLH(viewLeft, viewRight, viewBottom, viewTop, NearZ, FarZ);
}

void ShadowCamera::StabilizeCamera(float* viewLeft, float* viewRight, float* viewBottom, float* viewTop, 
	const std::array<DirectX::XMVECTOR, 8>& points)
{
	// Checks if all view points are not nullptr
	assert(viewLeft);
	assert(viewRight);
	assert(viewBottom);
	assert(viewTop);

	const auto calculatedDiagonalVector = DirectX::XMVector3Length(DirectX::XMVectorSubtract(points[3], points[4]));
	if (!DirectX::XMVector3NearEqual(calculatedDiagonalVector, LongestDiagonalVector, LONGEST_DIAGONAL_CALCULATION_EPSILON))
	{
		Logger::Debug("Preparing to recalculate shadow camera longest diagonal...");
		LongestDiagonalVector = calculatedDiagonalVector;
		LongestDiagonal = DirectX::XMVectorGetX(LongestDiagonalVector);
	}

	DirectX::XMFLOAT2 offsetFloats{};
	const auto offsetVector = DirectX::XMVectorScale(
		DirectX::XMVectorSubtract(
			LongestDiagonalVector,
			DirectX::XMVectorSubtract(
				DirectX::XMVectorSet(*viewRight, *viewTop, 0.0f, 0.0f),
				DirectX::XMVectorSet(*viewLeft, *viewBottom, 0.0f, 0.0f)
			)
		), 0.5f
	);
	XMStoreFloat2(&offsetFloats, offsetVector);

	*viewRight += offsetFloats.x;
	*viewTop += offsetFloats.y;

	*viewLeft -= offsetFloats.x;
	*viewBottom -= offsetFloats.y;

	const auto worldUnitsPerTexel = LongestDiagonal / ShadowResolution;

	*viewRight /= worldUnitsPerTexel;
	*viewRight = std::floor(*viewRight);
	*viewRight *= worldUnitsPerTexel;

	*viewTop /= worldUnitsPerTexel;
	*viewTop = std::floor(*viewTop);
	*viewTop *= worldUnitsPerTexel;

	*viewLeft /= worldUnitsPerTexel;
	*viewLeft = std::floor(*viewLeft);
	*viewLeft *= worldUnitsPerTexel;

	*viewBottom /= worldUnitsPerTexel;
	*viewBottom = std::floor(*viewBottom);
	*viewBottom *= worldUnitsPerTexel;
}
