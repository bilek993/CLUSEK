#include "PhysicsMaterialsWindow.h"
#include <imgui.h>

void PhysicsMaterialsWindow::Draw()
{
	ImGui::Begin("Physics materials", &IsEnabled);

	const auto physicsSystem = dynamic_cast<PhysicsSystem*>((*Systems)[PhysicsSystemId].System.get());

	DrawCombo(physicsSystem);
	DrawDetails(physicsSystem);

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

void PhysicsMaterialsWindow::DrawDetails(const PhysicsSystem* physicsSystem) const
{
	const auto physicsMaterialManager = physicsSystem->GetPhysicsMaterialManagerSmartPointer();
	const auto currentMaterial = physicsMaterialManager->GetMaterialById(SelectedId);

	auto staticFriction = currentMaterial->getStaticFriction();
	auto dynamicFriction = currentMaterial->getDynamicFriction();
	auto restitution = currentMaterial->getRestitution();

	ImGui::DragFloat("Static friction", &staticFriction, 0.001f, 0.0f, 1.0f);
	ImGui::DragFloat("Dynamic friction", &dynamicFriction, 0.001f, 0.0f, 1.0f);
	ImGui::DragFloat("Restitution", &restitution, 0.001f, 0.0f, 1.0f);

	currentMaterial->setStaticFriction(staticFriction);
	currentMaterial->setDynamicFriction(dynamicFriction);
	currentMaterial->setRestitution(restitution);
}
