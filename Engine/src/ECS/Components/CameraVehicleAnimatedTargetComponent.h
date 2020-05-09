#pragma once
#include <DirectXMath.h>

struct CameraVehicleAnimatedTargetComponent final
{
	float Power = 0.1f;
	float Smoothness = 0.001f;
	DirectX::XMVECTOR AnimationPositionVector;

	DirectX::XMVECTOR CurrentAnimationPositionVector;
};
