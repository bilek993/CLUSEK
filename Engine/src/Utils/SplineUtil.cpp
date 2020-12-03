#include "SplineUtil.h"
#include "Logger.h"

std::vector<DirectX::XMVECTOR> SplineUtil::CalculateEvenlySpaceLookUpTable(const int resolution, const float distance,
                                                                           const std::function<DirectX::XMVECTOR(float)>& generatorFunction)
{
	std::vector<DirectX::XMVECTOR> lookUpTable{};
	auto distanceFromLastPoint = distance;

	DirectX::XMVECTOR previousVector{};
	
	for (auto i = 0; i < resolution; i++)
	{
		const auto t = static_cast<float>(i) / static_cast<float>(resolution);
		const auto currentVector = generatorFunction(t);

		if (i != 0) // Not on first run
			distanceFromLastPoint += DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(previousVector, currentVector)));

		while (distanceFromLastPoint >= distance)
		{
			distanceFromLastPoint -= distance;
			lookUpTable.emplace_back(currentVector);
		}

		previousVector = currentVector;
	}

	return lookUpTable;
}

DirectX::XMVECTOR SplineUtil::CalculateBezierQuadraticCurve(const DirectX::XMVECTOR& a, const DirectX::XMVECTOR& b,
                                                            const DirectX::XMVECTOR& c, const float t)
{
	const auto p0 = DirectX::XMVectorLerp(a, b, t);
	const auto p1 = DirectX::XMVectorLerp(b, c, t);
	return DirectX::XMVectorLerp(p0, p1, t);;
}

DirectX::XMFLOAT4 SplineUtil::CalculateBezierQuadraticCurve(const DirectX::XMFLOAT4& a, const DirectX::XMFLOAT4& b,
	const DirectX::XMFLOAT4& c, const float t)
{
	const auto aVector = XMLoadFloat4(&a);
	const auto bVector = XMLoadFloat4(&b);
	const auto cVector = XMLoadFloat4(&c);

	DirectX::XMFLOAT4 result{};
	XMStoreFloat4(&result, CalculateBezierQuadraticCurve(aVector, bVector, cVector, t));

	return result;
}

DirectX::XMFLOAT3 SplineUtil::CalculateBezierQuadraticCurve(const DirectX::XMFLOAT3& a, const DirectX::XMFLOAT3& b,
	const DirectX::XMFLOAT3& c, const float t)
{
	const auto aVector = XMLoadFloat3(&a);
	const auto bVector = XMLoadFloat3(&b);
	const auto cVector = XMLoadFloat3(&c);

	DirectX::XMFLOAT3 result{};
	XMStoreFloat3(&result, CalculateBezierQuadraticCurve(aVector, bVector, cVector, t));

	return result;
}

DirectX::XMFLOAT2 SplineUtil::CalculateBezierQuadraticCurve(const DirectX::XMFLOAT2& a, const DirectX::XMFLOAT2& b,
	const DirectX::XMFLOAT2& c, const float t)
{
	const auto aVector = XMLoadFloat2(&a);
	const auto bVector = XMLoadFloat2(&b);
	const auto cVector = XMLoadFloat2(&c);

	DirectX::XMFLOAT2 result{};
	XMStoreFloat2(&result, CalculateBezierQuadraticCurve(aVector, bVector, cVector, t));

	return result;
}

DirectX::XMVECTOR SplineUtil::CalculateBezierCubicCurve(const DirectX::XMVECTOR& a, const DirectX::XMVECTOR& b,
	const DirectX::XMVECTOR& c, const DirectX::XMVECTOR& d, const float t)
{
	const auto p0 = CalculateBezierQuadraticCurve(a, b, c, t);
	const auto p1 = CalculateBezierQuadraticCurve(b, c, d, t);
	return DirectX::XMVectorLerp(p0, p1, t);
}

DirectX::XMFLOAT4 SplineUtil::CalculateBezierCubicCurve(const DirectX::XMFLOAT4& a, const DirectX::XMFLOAT4& b,
	const DirectX::XMFLOAT4& c, const DirectX::XMFLOAT4& d, const float t)
{
	const auto aVector = XMLoadFloat4(&a);
	const auto bVector = XMLoadFloat4(&b);
	const auto cVector = XMLoadFloat4(&c);
	const auto dVector = XMLoadFloat4(&d);

	DirectX::XMFLOAT4 result{};
	XMStoreFloat4(&result, CalculateBezierCubicCurve(aVector, bVector, cVector, dVector, t));

	return result;
}

DirectX::XMFLOAT3 SplineUtil::CalculateBezierCubicCurve(const DirectX::XMFLOAT3& a, const DirectX::XMFLOAT3& b,
	const DirectX::XMFLOAT3& c, const DirectX::XMFLOAT3& d, const float t)
{
	const auto aVector = XMLoadFloat3(&a);
	const auto bVector = XMLoadFloat3(&b);
	const auto cVector = XMLoadFloat3(&c);
	const auto dVector = XMLoadFloat3(&d);

	DirectX::XMFLOAT3 result{};
	XMStoreFloat3(&result, CalculateBezierCubicCurve(aVector, bVector, cVector, dVector, t));

	return result;
}

DirectX::XMFLOAT2 SplineUtil::CalculateBezierCubicCurve(const DirectX::XMFLOAT2& a, const DirectX::XMFLOAT2& b,
	const DirectX::XMFLOAT2& c, const DirectX::XMFLOAT2& d, const float t)
{
	const auto aVector = XMLoadFloat2(&a);
	const auto bVector = XMLoadFloat2(&b);
	const auto cVector = XMLoadFloat2(&c);
	const auto dVector = XMLoadFloat2(&d);

	DirectX::XMFLOAT2 result{};
	XMStoreFloat2(&result, CalculateBezierCubicCurve(aVector, bVector, cVector, dVector, t));

	return result;
}

void SplineUtil::RecalculateNextControlPoint(const DirectX::XMVECTOR& currentAnchorPoint, DirectX::XMVECTOR& nextControlPoint, 
	const DirectX::XMVECTOR& secondNextControlPoint)
{
	const auto direction = DirectX::XMVectorSubtract(secondNextControlPoint, currentAnchorPoint);
	nextControlPoint = DirectX::XMVectorAdd(DirectX::XMVectorScale(direction, 0.5f), currentAnchorPoint);
}

void SplineUtil::RecalculatePreviousControlPoint(const DirectX::XMVECTOR& currentAnchorPoint,
	DirectX::XMVECTOR& previousControlPoint, const DirectX::XMVECTOR& secondPreviousControlPoint)
{
	const auto direction = DirectX::XMVectorSubtract(secondPreviousControlPoint, currentAnchorPoint);
	previousControlPoint = DirectX::XMVectorAdd(DirectX::XMVectorScale(direction, 0.5f), currentAnchorPoint);
}

void SplineUtil::RecalculateMiddleControlPoint(const DirectX::XMVECTOR& currentAnchorPoint,	const DirectX::XMVECTOR& previousAnchorPoint, 
	const DirectX::XMVECTOR& nextAnchorPoint, DirectX::XMVECTOR* previousControlPoint, DirectX::XMVECTOR* nextControlPoint)
{
	const auto directionToPreviousAnchorPoint = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(previousAnchorPoint, currentAnchorPoint));
	const auto directionToNextAnchorPoint = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(nextAnchorPoint, currentAnchorPoint));

	const auto halfDistanceToPreviousAnchorPoint = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(previousAnchorPoint, currentAnchorPoint))) * 0.5f;
	const auto halfDistanceToNextAnchorPoint = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(nextAnchorPoint, currentAnchorPoint))) * 0.5f;

	const auto previousControlPointDirection = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(directionToPreviousAnchorPoint, directionToNextAnchorPoint));
	const auto nextControlPointDirection = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(directionToNextAnchorPoint, directionToPreviousAnchorPoint));

	if (previousControlPoint != nullptr)
		*previousControlPoint = DirectX::XMVectorAdd(DirectX::XMVectorScale(previousControlPointDirection, halfDistanceToPreviousAnchorPoint), currentAnchorPoint);
	if (nextControlPoint != nullptr)
		*nextControlPoint = DirectX::XMVectorAdd(DirectX::XMVectorScale(nextControlPointDirection, halfDistanceToNextAnchorPoint), currentAnchorPoint);
}
