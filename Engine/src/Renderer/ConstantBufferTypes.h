#pragma once
#include <DirectXMath.h>

struct CB_UberVertexShader
{
	DirectX::XMMATRIX WorldViewProjectionMat;
	DirectX::XMMATRIX WorldMatrix;
};

struct CB_UberPixelShader
{
	DirectX::XMFLOAT3 AmbientLightColor;
	float AmbientLightStrength;
	DirectX::XMFLOAT3 DirectionalLightColor;
	float DirectionalLightStrength;
	DirectX::XMFLOAT3 DirectionalLightDirection;
	float Alpha;
};

struct CB_SkyVertexShader
{
	DirectX::XMMATRIX WorldViewProjectionMat;
};