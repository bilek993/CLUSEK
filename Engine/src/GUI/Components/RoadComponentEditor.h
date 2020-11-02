#pragma once
#include "BaseComponentEditor.h"
#include "../../ECS/Components/RoadComponent.h"

class RoadComponentEditor final : public BaseComponentEditor<RoadComponent>
{
protected:
	void Draw() override;
};