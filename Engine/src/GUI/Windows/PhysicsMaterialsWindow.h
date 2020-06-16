#pragma once
#include "BaseWindow.h"
#include "../../ECS/Systems/PhysicsSystem.h"

class PhysicsMaterialsWindow final : public BaseWindow
{
protected:
	void Draw() override;

private:
	void DrawCombo(const PhysicsSystem* physicsSystem);

	int SelectedId = 0;
};
