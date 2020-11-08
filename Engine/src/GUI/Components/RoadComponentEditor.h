#pragma once
#include "BaseComponentEditor.h"
#include "../../ECS/Components/RoadComponent.h"
#include "../../ECS/Components/TransformComponent.h"

class RoadComponentEditor final : public BaseComponentEditor<RoadComponent>
{
protected:
	void Draw() override;

private:
	void DrawPoints(RoadComponent* componentPointer);
	void DrawControlButtons(RoadComponent* componentPointer) const;

	void DrawPoints(RoadComponent* componentPointer, const DirectX::XMMATRIX& viewProjectionMatrix);
	void DrawConnectionLines(RoadComponent* componentPointer, const TransformComponent& cameraTransform, const DirectX::XMMATRIX& viewProjectionMatrix) const;
	void DrawGizmos(RoadComponent* componentPointer, const DirectX::XMMATRIX& viewMatrix, const DirectX::XMMATRIX& projectionMatrix) const;

	ImVec2 FixVectorOutsideCameraPlanesIfNeeded(const ImVec2& pointToBeFixed, const ImVec2& secondPoint, 
		const DirectX::XMFLOAT3& cameraPosition, const DirectX::XMFLOAT3& pointToBeFixedWorldPosition, bool outsideCameraPlanes) const;

	int SelectedPointId = -1;

	inline const static float DOT_SIZE = 7.0f;
	inline const static ImColor DOT_COLOR_PRIMARY = ImColor(255, 255, 255);
	inline const static ImColor DOT_COLOR_SECONDARY = ImColor(175, 175, 175);

	inline const static ImColor LINE_COLOR_PRIMARY = ImColor(255, 255, 255, 255);
	inline const static ImColor LINE_COLOR_SECONDARY = ImColor(255, 255, 255, 100);
};
