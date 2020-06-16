#include "PhysicsMaterialsWindow.h"
#include <imgui.h>
#include "../../ECS/Systems/PhysicsSystem.h"

void PhysicsMaterialsWindow::Draw()
{
	ImGui::Begin("Physics materials", &IsEnabled);

	const auto physicsSystem = dynamic_cast<PhysicsSystem*>((*Systems)[PhysicsSystemId].System.get());

	ImGui::End();
}
