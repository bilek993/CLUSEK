#include "GuiTransformUtil.h"
#include <cmath>

ImVec2 GuiTransformUtil::TransformWorldPositionToScreenPoint(const DirectX::XMFLOAT3& worldPoint, const DirectX::XMMATRIX& viewProjectionMatrix, 
	bool* visible, bool* outsideCameraPlanes, const float farPlane, const float nearPlane)
{
	DirectX::XMFLOAT4 worldPoint4(worldPoint.x, worldPoint.y, worldPoint.z, 1.0f);
	const auto worldPoint4Vector = XMLoadFloat4(&worldPoint4);
	
	return TransformWorldPositionToScreenPoint(	worldPoint4Vector, 
												viewProjectionMatrix, 
												visible,
												outsideCameraPlanes, 
												farPlane, 
												nearPlane);
}

ImVec2 GuiTransformUtil::TransformWorldPositionToScreenPoint(const DirectX::XMFLOAT4& worldPoint, const DirectX::XMMATRIX& viewProjectionMatrix, 
	bool* visible, bool* outsideCameraPlanes, const float farPlane, const float nearPlane)
{
	const auto worldPointVector = XMLoadFloat4(&worldPoint);
	
	return TransformWorldPositionToScreenPoint(	worldPointVector, 
												viewProjectionMatrix, 
												visible,
												outsideCameraPlanes, 
												farPlane, 
												nearPlane);
}

ImVec2 GuiTransformUtil::TransformWorldPositionToScreenPoint(const DirectX::XMVECTOR& worldPoint, const DirectX::XMMATRIX& viewProjectionMatrix, 
	bool* visible, bool* outsideCameraPlanes, const float farPlane, const float nearPlane)
{
	const auto viewportSize = ImGui::GetMainViewport()->Size;
	const auto viewportPos = ImGui::GetMainViewport()->Pos;
	
	auto resultVector = XMVector4Transform(worldPoint, viewProjectionMatrix);
	
	const auto depth = DirectX::XMVectorGetZ(resultVector);
	
	resultVector = DirectX::XMVectorScale(resultVector, 0.5f / DirectX::XMVectorGetW(resultVector));

	DirectX::XMFLOAT3 resultFloats{};
	XMStoreFloat3(&resultFloats, resultVector);

	resultFloats.x += 0.5f;
	resultFloats.y += 0.5f;

	resultFloats.y = 1.0f - resultFloats.y;

	resultFloats.x *= viewportSize.x;
	resultFloats.y *= viewportSize.y;

	resultFloats.x += viewportPos.x;
	resultFloats.y += viewportPos.y;

	if (outsideCameraPlanes != nullptr)
		*outsideCameraPlanes = depth < nearPlane || depth > farPlane;

	if (visible != nullptr)
	{
		*visible = !(	resultFloats.x < 0 || 
						resultFloats.y < 0 ||
						resultFloats.x > viewportSize.x || 
						resultFloats.y > viewportSize.y || 
						depth < nearPlane || 
						depth > farPlane);
	}
	
	return ImVec2(resultFloats.x, resultFloats.y);
}

bool GuiTransformUtil::IsTouching(const ImVec2& objectCenterPosition, const ImVec2& mousePosition, float maxDistance)
{
	const auto xDifference = objectCenterPosition.x - mousePosition.x;
	const auto yDifference = objectCenterPosition.y - mousePosition.y;
	
	const auto distance = std::sqrtf((xDifference * xDifference) + (yDifference * yDifference));

	return distance < maxDistance;
}
