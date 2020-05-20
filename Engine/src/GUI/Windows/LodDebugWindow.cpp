#include "LodDebugWindow.h"
#include <imgui.h>

void LodDebugWindow::Draw()
{
	ImGui::Begin("LOD Debug", &IsEnabled);

	ImGui::Combo("Mode", reinterpret_cast<int*>(&RenderSettings->LevelOfDetailsMode), "Automatic\0Manual\0\0");

	if (RenderSettings->LevelOfDetailsMode == Manual)
		ImGui::SliderFloat("Forced percentage", &RenderSettings->ForcedLodPercentage, 0.0f, 1.0f);

	ImGui::Separator();
	ImGui::Text("Switching mode from 'Automatic' to 'Manual' may cause visual glitches!");

	ImGui::End();
}
