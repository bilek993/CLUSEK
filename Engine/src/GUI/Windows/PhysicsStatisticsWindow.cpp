#include "PhysicsStatisticsWindow.h"
#include <imgui.h>

void PhysicsStatisticsWindow::Draw()
{
	ImGui::Begin("Physics statistics", &IsEnabled);

	const auto physicsSystem = FindPhysicsSystem();

	if (!physicsSystem)
		DrawNotImplementedInfo();
	else
		DrawStats();

	ImGui::End();
}

PhysicsSystem* PhysicsStatisticsWindow::FindPhysicsSystem() const
{
	for (auto &system : *Systems)
	{
		const auto physicsSystem = dynamic_cast<PhysicsSystem*>(system.System.get());
		if (physicsSystem)
			return physicsSystem;
	}

	return nullptr;
}

void PhysicsStatisticsWindow::DrawNotImplementedInfo() const
{
	ImGui::Text("Physics System not found!");
	ImGui::Text("Add at least one physics system to your project.");
}

void PhysicsStatisticsWindow::DrawStats() const
{
	ImGui::Text("DrawStats()");
}
