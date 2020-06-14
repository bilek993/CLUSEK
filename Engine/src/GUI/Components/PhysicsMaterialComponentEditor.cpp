#include "PhysicsMaterialComponentEditor.h"

void PhysicsMaterialComponentEditor::Draw()
{
	const auto component = GetPointerToThisComponent();

	ImGui::Text("This component is connected with '%s' material.", component->Name.c_str());
}
