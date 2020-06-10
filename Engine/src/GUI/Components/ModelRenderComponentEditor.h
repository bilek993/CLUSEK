#pragma once
#include "BaseComponentEditor.h"
#include "../../ECS/Components/ModelRenderComponent.h"

class ModelRenderComponentEditor final : public BaseComponentEditor<ModelRenderComponent>
{
protected:
	void Draw() override;
};
