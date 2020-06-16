#include "PhysicsMaterialsWindow.h"
#include <imgui.h>

void PhysicsMaterialsWindow::Draw()
{
	ImGui::Begin("Physics materials", &IsEnabled);

	const auto physicsSystem = dynamic_cast<PhysicsSystem*>((*Systems)[PhysicsSystemId].System.get());

	DrawCombo(physicsSystem);

	ImGui::End();
}

void PhysicsMaterialsWindow::DrawCombo(const PhysicsSystem* physicsSystem)
{
	const auto physicsMaterialManager = physicsSystem->GetPhysicsMaterialManagerSmartPointer();
	std::string materialNames{};

	for (auto& materialName : physicsMaterialManager->GetPhysicsMaterialNames())
		materialNames += materialName + '\0';

	ImGui::Combo("Selected material", &SelectedId, materialNames.c_str());
}
