#pragma once
#include <DirectXMath.h>

struct FatPerObjectBuffer final
{
	DirectX::XMMATRIX WorldViewProjectionMat;
	DirectX::XMMATRIX WorldMatrix;
};
