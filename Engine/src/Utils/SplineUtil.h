#pragma once
#include <DirectXMath.h>

class SplineUtil final
{
public:
	static DirectX::XMVECTOR CalculateBezierQuadraticCurve(const DirectX::XMVECTOR& a, const DirectX::XMVECTOR& b, 
		const DirectX::XMVECTOR& c, float t);
	static DirectX::XMFLOAT4 CalculateBezierQuadraticCurve(const DirectX::XMFLOAT4& a, const DirectX::XMFLOAT4& b, 
		const DirectX::XMFLOAT4& c, float t);
	static DirectX::XMFLOAT3 CalculateBezierQuadraticCurve(const DirectX::XMFLOAT3& a, const DirectX::XMFLOAT3& b, 
		const DirectX::XMFLOAT3& c, float t);
	static DirectX::XMFLOAT2 CalculateBezierQuadraticCurve(const DirectX::XMFLOAT2& a, const DirectX::XMFLOAT2& b, 
		const DirectX::XMFLOAT2& c, float t);

	static DirectX::XMVECTOR CalculateBezierCubicCurve(const DirectX::XMVECTOR& a, const DirectX::XMVECTOR& b,
		const DirectX::XMVECTOR& c, const DirectX::XMVECTOR& d, float t);
	static DirectX::XMFLOAT4 CalculateBezierCubicCurve(const DirectX::XMFLOAT4& a, const DirectX::XMFLOAT4& b,
		const DirectX::XMFLOAT4& c, const DirectX::XMFLOAT4& d, float t);
	static DirectX::XMFLOAT3 CalculateBezierCubicCurve(const DirectX::XMFLOAT3& a, const DirectX::XMFLOAT3& b, 
		const DirectX::XMFLOAT3& c, const DirectX::XMFLOAT3& d, float t);
	static DirectX::XMFLOAT2 CalculateBezierCubicCurve(const DirectX::XMFLOAT2& a, const DirectX::XMFLOAT2& b, 
		const DirectX::XMFLOAT2& c, const DirectX::XMFLOAT2& d, float t);
};
