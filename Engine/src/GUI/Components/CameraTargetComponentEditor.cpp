#include "CameraTargetComponentEditor.h"

void CameraTargetComponentEditor::Draw()
{
	const auto component = GetPointerToThisComponent();

	ImGui::Checkbox("Active", &component->Active);
	ImGui::InputFloat("Rotation lag", &component->RotationLag, 0, 0, "%.5f");

	HandleIncorrectValues(component);
}

void CameraTargetComponentEditor::HandleIncorrectValues(CameraTargetComponent* component) const
{
	component->RotationLag = std::min(component->RotationLag, std::numeric_limits<float>::max());
	component->RotationLag = std::max(component->RotationLag, 0.0f);
}
