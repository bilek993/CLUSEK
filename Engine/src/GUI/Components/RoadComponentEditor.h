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
};