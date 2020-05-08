#pragma once
#include <DirectXMath.h>

struct CameraTargetComponent final
{
	bool Active = false;
	float RotationLag = 0.001f;
	DirectX::XMFLOAT3 InitialPositionVector;
	DirectX::XMFLOAT3 InitialRotationVector;

	DirectX::XMMATRIX PaddingMatrix;
};
