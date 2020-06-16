#pragma once
#include "BaseWindow.h"
#include "../../ECS/Systems/PhysicsSystem.h"

class PhysicsMaterialsWindow final : public BaseWindow
{
protected:
	void Draw() override;

private:
	void DrawCombo(const PhysicsSystem* physicsSystem);
	void DrawDetails(const PhysicsSystem* physicsSystem) const;

	int SelectedId = 0;
};
