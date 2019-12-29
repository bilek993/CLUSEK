#pragma once
#include <DirectXMath.h>

class ShadowCamera final
{
public:
	ShadowCamera();

	void UpdateLightDirection(float lightDirectionX, float lightDirectionY, float lightDirectionZ);
	void UpdateShadowMapLocation();

	DirectX::XMMATRIX CalculateCameraMatrix() const;
private:
	DirectX::XMVECTOR UpVector{};
	DirectX::XMVECTOR EyeVector{};

	DirectX::XMMATRIX ProjectionMatrix{};

	DirectX::XMFLOAT3 LightDirection{};
	DirectX::XMMATRIX ViewMatrix{};
};
