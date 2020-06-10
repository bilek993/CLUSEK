#include "ModelRenderComponentEditor.h"

void ModelRenderComponentEditor::Draw()
{
	const auto component = GetPointerToThisComponent();

	ImGui::Checkbox("Invisible", &component->Invisible);
	ImGui::InputFloat("Low poly distance", &component->LowPolyDistance);
	ImGui::InputFloat("Lod transition distance", &component->LodTransitionDistance);
}
