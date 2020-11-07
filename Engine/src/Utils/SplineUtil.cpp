#include "SplineUtil.h"

DirectX::XMVECTOR SplineUtil::CalculateBezierQuadraticCurve(const DirectX::XMVECTOR& a, const DirectX::XMVECTOR& b,
	const DirectX::XMVECTOR& c, const float t)
{
	const auto p0 = DirectX::XMVectorLerp(a, b, t);
	const auto p1 = DirectX::XMVectorLerp(b, c, t);
	return DirectX::XMVectorLerp(p0, p1, t);;
}

DirectX::XMVECTOR SplineUtil::CalculateBezierCubicCurve(const DirectX::XMVECTOR& a, const DirectX::XMVECTOR& b,
	const DirectX::XMVECTOR& c, const DirectX::XMVECTOR& d, const float t)
{
	const auto p0 = CalculateBezierQuadraticCurve(a, b, c, t);
	const auto p1 = CalculateBezierQuadraticCurve(b, c, d, t);
	return DirectX::XMVectorLerp(p0, p1, t);
}