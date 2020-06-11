#include "VehiclePlayerControllerComponentEditor.h"

void VehiclePlayerControllerComponentEditor::Draw()
{
	const auto component = GetPointerToThisComponent();

	ImGui::Checkbox("Possessed", &component->Possessed);
	ImGui::InputFloat("Steering speed", &component->SteeringSpeed);
	ImGui::InputFloat("Wheel returning to neutral position", &component->WheelReturningToNeutralPosition, 0, 0, "%.5f");
	ImGui::InputFloat("Minimal speed for gear change from or to reverse", &component->MinimalSpeedForGearChangeFromOrToReverse);
}
