#pragma once
#include <DirectXMath.h>

struct CB_VS_UberVertexShader
{
	DirectX::XMMATRIX MatModelViewProjection;
};

struct CB_PS_Light_UberVertexShader
{
	DirectX::XMFLOAT3 AmbientLightColor;
	float AmbientLightStrength;
};