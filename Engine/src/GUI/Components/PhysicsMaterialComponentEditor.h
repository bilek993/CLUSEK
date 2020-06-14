#pragma once
#include "BaseComponentEditor.h"
#include "../../ECS/Components/PhysicsMaterialComponent.h"

class PhysicsMaterialComponentEditor final : public BaseComponentEditor<PhysicsMaterialComponent>
{
protected:
	void Draw() override;
};
