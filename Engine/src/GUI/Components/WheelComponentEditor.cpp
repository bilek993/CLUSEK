#include "WheelComponentEditor.h"

void WheelComponentEditor::Draw()
{
	const auto component = GetPointerToThisComponent();

	ImGui::Checkbox("Flip", &component->Flip);
}
