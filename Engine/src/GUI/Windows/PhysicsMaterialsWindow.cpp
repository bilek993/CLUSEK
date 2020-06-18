#include "PhysicsMaterialsWindow.h"
#include <imgui.h>
#include "../../Utils/FloatingPointComparator.h"

void PhysicsMaterialsWindow::Draw()
{
	ImGui::Begin("Physics materials", &IsEnabled);

	const auto physicsSystem = dynamic_cast<PhysicsSystem*>((*Systems)[PhysicsSystemId].System.get());

	DrawCombo(physicsSystem);
	DrawBasicMaterialDetails(physicsSystem);
	DrawFrictionPairDetails(physicsSystem);

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

void PhysicsMaterialsWindow::DrawBasicMaterialDetails(const PhysicsSystem* physicsSystem) const
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

void PhysicsMaterialsWindow::DrawFrictionPairDetails(PhysicsSystem* physicsSystem) const
{
	const auto physicsMaterialManager = physicsSystem->GetPhysicsMaterialManagerSmartPointer();
	auto tireFriction = physicsMaterialManager->GetTireFrictionById(SelectedId);

	ImGui::DragFloat("Tire friction", &tireFriction, 0.001f, 0.0f, 1.0f);

	// Prevents unnecessary calculations
	if (!FloatingPointComparator::IsAlmostEqual(tireFriction, physicsMaterialManager->GetTireFrictionById(SelectedId)))
	{
		physicsMaterialManager->UpdateTireFrictionById(SelectedId, tireFriction);
		physicsSystem->UpdateFrictionPairs();
	}
}
