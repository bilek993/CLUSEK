#pragma once
#include <DirectXMath.h>

struct CameraTargetComponent final
{
	bool Active = false;
	DirectX::XMFLOAT3 InitialPositionVector;
	DirectX::XMFLOAT3 InitialRotationVector;

	DirectX::XMMATRIX PaddingMatrix;
};
