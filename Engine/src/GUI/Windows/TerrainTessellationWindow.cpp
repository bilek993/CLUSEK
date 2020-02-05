#include "TerrainTessellationWindow.h"
#include <imgui.h>

void TerrainTessellationWindow::Draw()
{
	ImGui::Begin("Terrain tessellation settings", &IsEnabled);

	ImGui::SliderFloat("Minimal tessellation factor", &RenderSettings->MinTerrainTessellationFactor, 0.0f, 6.0f);
	ImGui::SliderFloat("Maximum tessellation factor", &RenderSettings->MaxTerrainTessellationFactor, 0.0f, 6.0f);

	ImGui::DragFloat("Minimum tessellation distance", &RenderSettings->MinTerrainTessellationDistance, 5.0f);
	ImGui::DragFloat("Maximum tessellation distance", &RenderSettings->MaxTerrainTessellationDistance, 5.0f);

	ImGui::End();
}
