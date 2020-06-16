#pragma once
#include "BaseWindow.h"
#include "../../ECS/Systems/PhysicsSystem.h"

class PhysicsStatisticsWindow final : public BaseWindow
{
protected:
	void Draw() override;

private:
	void DrawNotImplementedInfo() const;
	void DrawStats(const PhysicsSystem* physicsSystem) const;
};
