#pragma once
#include "BaseComponentEditor.h"
#include "../../ECS/Components/RoadComponent.h"

class RoadComponentEditor final : public BaseComponentEditor<RoadComponent>
{
protected:
	void Draw() override;

private:
	void DrawPoints(RoadComponent* componentPointer) const;
	void DrawControlButtons(RoadComponent* componentPointer) const;

	void DrawPointsWithGizmosOnScreen(RoadComponent* componentPointer) const;

	inline const static ImColor DOT_COLOR = ImColor(255, 255, 255);
};