#include "TerrainComponentEditor.h"

void TerrainComponentEditor::Draw()
{
	const auto component = GetPointerToThisComponent();

	ImGui::SliderFloat("Minimal tessellation factor", &component->MinTerrainTessellationFactor, 0.0f, 6.0f);
	ImGui::SliderFloat("Maximum tessellation factor", &component->MaxTerrainTessellationFactor, 0.0f, 6.0f);

	ImGui::Separator();

	ImGui::DragFloat("Minimum tessellation distance", &component->MinTerrainTessellationDistance, 5.0f);
	ImGui::DragFloat("Maximum tessellation distance", &component->MaxTerrainTessellationDistance, 5.0f);

	ImGui::Separator();

	ImGui::DragFloat("Grass placement threshold", &component->GrassPlacementThreshold, 5.0f);
}
