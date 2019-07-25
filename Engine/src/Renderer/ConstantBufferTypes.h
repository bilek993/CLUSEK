#pragma once
#include <DirectXMath.h>

struct CB_VS_PerObjectBuffer_UberVertexShader
{
	DirectX::XMMATRIX WorldViewProjectionMat;
	DirectX::XMMATRIX WorldMatrix;
};

struct CB_PS_Light_UberVertexShader
{
	DirectX::XMFLOAT3 AmbientLightColor;
	float AmbientLightStrength;
};