#pragma once
#include <DirectXMath.h>
#include <imgui.h>

class GuiTransformUtil final
{
public:
	static ImVec2 TransformWorldPositionToScreenPoint(const DirectX::XMFLOAT3& worldPoint, const DirectX::XMMATRIX& viewProjectionMatrix, 
		bool& outsideTheScreen, float farPlane, float nearPlane);
	static ImVec2 TransformWorldPositionToScreenPoint(const DirectX::XMFLOAT4& worldPoint, const DirectX::XMMATRIX& viewProjectionMatrix, 
		bool& outsideTheScreen, float farPlane, float nearPlane);
	static ImVec2 TransformWorldPositionToScreenPoint(const DirectX::XMVECTOR& worldPoint, const DirectX::XMMATRIX& viewProjectionMatrix, 
		bool& outsideTheScreen, float farPlane, float nearPlane);

	static bool IsTouching(const ImVec2& objectCenterPosition, const ImVec2& mousePosition, float maxDistance);
};
