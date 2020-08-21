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

	ImGui::Separator();

	ImGui::InputFloat("Min grass translation X", &RenderSettings->MinGrassTranslationX);
	ImGui::InputFloat("Min grass translation Y", &RenderSettings->MinGrassTranslationY);
	ImGui::InputFloat("Max grass translation X", &RenderSettings->MaxGrassTranslationX);
	ImGui::InputFloat("Max grass translation Y", &RenderSettings->MaxGrassTranslationY);

	ImGui::Separator();

	ImGui::SliderFloat("Grass end cascade 0", &RenderSettings->GrassEndCascade0, 0.0f, 1.0f);
	ImGui::SliderFloat("Grass end cascade 1", &RenderSettings->GrassEndCascade1, 0.0f, 1.0f);
	ImGui::SliderFloat("Grass end", &RenderSettings->GrassEnd, 0.0f, 1.0f);
	
	ImGui::End();

	FixValues();
}

void GrassWindow::FixValues() const
{
	RenderSettings->GrassEndCascade0 = std::min(RenderSettings->GrassEndCascade0, RenderSettings->GrassEndCascade1);
	RenderSettings->GrassEndCascade1 = std::min(RenderSettings->GrassEndCascade1, RenderSettings->GrassEnd);
}
