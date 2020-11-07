#pragma once
#include "BaseComponentEditor.h"
#include "../../ECS/Components/RoadComponent.h"

class RoadComponentEditor final : public BaseComponentEditor<RoadComponent>
{
protected:
	void Draw() override;

private:
	void DrawPoints(RoadComponent* componentPointer);
	void DrawControlButtons(RoadComponent* componentPointer) const;

	void DrawPoints(RoadComponent* componentPointer, const DirectX::XMMATRIX& viewProjectionMatrix);
	void DrawGizmos(RoadComponent* componentPointer, const DirectX::XMMATRIX& viewMatrix, const DirectX::XMMATRIX& projectionMatrix) const;

	int SelectedPointId = -1;

	inline const static float DOT_SIZE = 7.0f;
	inline const static ImColor DOT_COLOR_PRIMARY = ImColor(255, 255, 255);
	inline const static ImColor DOT_COLOR_SECONDARY = ImColor(175, 175, 175);
};