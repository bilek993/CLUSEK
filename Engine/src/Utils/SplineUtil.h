#pragma once
#include <DirectXMath.h>

class SplineUtil final
{
public:
	static DirectX::XMVECTOR CalculateBezierQuadraticCurve(const DirectX::XMVECTOR& a, const DirectX::XMVECTOR& b, 
		const DirectX::XMVECTOR& c, float t);

	static DirectX::XMVECTOR CalculateBezierCubicCurve(const DirectX::XMVECTOR& a, const DirectX::XMVECTOR& b,
		const DirectX::XMVECTOR& c, const DirectX::XMVECTOR& d, float t);
};
