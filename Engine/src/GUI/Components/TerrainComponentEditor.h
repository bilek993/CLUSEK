#pragma once
#include "BaseComponentEditor.h"
#include "../../ECS/Components/TerrainComponent.h"

class TerrainComponentEditor final : public BaseComponentEditor<TerrainComponent>
{
protected:
	void Draw() override;
};
