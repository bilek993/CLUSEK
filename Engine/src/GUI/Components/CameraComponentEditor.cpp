#include "CameraComponentEditor.h"

void CameraComponentEditor::Draw()
{
	const auto component = GetPointerToThisComponent();

	ImGui::Checkbox("Use target", &component->UseTarget);
}
