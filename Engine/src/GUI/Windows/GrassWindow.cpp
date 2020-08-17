#include "GrassWindow.h"

#include <imgui.h>

void GrassWindow::Draw()
{
	ImGui::Begin("Grass settings", &IsEnabled);

	ImGui::Checkbox("Enabled", &RenderSettings->GrassEnabled);

	ImGui::Separator();

	ImGui::InputFloat("Generator surface width", &RenderSettings->GrassGeneratorSurfaceWidth);
	ImGui::InputFloat("Generator surface height", &RenderSettings->GrassGeneratorSurfaceHeight);
	ImGui::InputFloat("Generator max camera distance", &RenderSettings->GrassGeneratorMaxCameraDistance);
	
	ImGui::End();
}
