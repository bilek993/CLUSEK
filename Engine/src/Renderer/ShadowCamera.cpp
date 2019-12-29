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

// TODO: Refactor this function in future
void ShadowCamera::UpdateShadowMapLocation()
{
	ProjectionMatrix = DirectX::XMMatrixOrthographicOffCenterLH(-100.0f, 100.0f, -100.0f, 100.0f, -100.0f, 100.0f);
}

DirectX::XMMATRIX ShadowCamera::CalculateCameraMatrix() const
{
	return ViewMatrix * ProjectionMatrix;
}
