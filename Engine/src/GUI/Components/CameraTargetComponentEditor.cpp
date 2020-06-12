#include "CameraTargetComponentEditor.h"

void CameraTargetComponentEditor::Draw()
{
	const auto component = GetPointerToThisComponent();

	ImGui::Checkbox("Active", &component->Active);
	ImGui::InputFloat("Rotation lag", &component->RotationLag, 0, 0, "%.5f");
}
