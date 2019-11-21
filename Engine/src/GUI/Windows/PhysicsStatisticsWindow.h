#pragma once
#include "BaseWindow.h"
#include "../../ECS/Systems/PhysicsSystem.h"

class PhysicsStatisticsWindow final : public BaseWindow
{
protected:
	void Draw() override;

private:
	PhysicsSystem* FindPhysicsSystem() const;

	void DrawNotImplementedInfo() const;
	void DrawStats() const;
};
