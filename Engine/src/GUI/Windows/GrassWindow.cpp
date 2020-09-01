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

	if (ImGui::SliderFloat("Grass end cascade 0", &RenderSettings->GrassEndCascade0, 0.0f, 1.0f))
	{
		RenderSettings->GrassEndCascade1 = std::max(RenderSettings->GrassEndCascade0, RenderSettings->GrassEndCascade1);
		RenderSettings->GrassEnd = std::max(RenderSettings->GrassEndCascade1, RenderSettings->GrassEnd);
	}
	if (ImGui::SliderFloat("Grass end cascade 1", &RenderSettings->GrassEndCascade1, 0.0f, 1.0f))
	{
		RenderSettings->GrassEndCascade0 = std::min(RenderSettings->GrassEndCascade0, RenderSettings->GrassEndCascade1);
		RenderSettings->GrassEnd = std::max(RenderSettings->GrassEndCascade1, RenderSettings->GrassEnd);
	}
	if(ImGui::SliderFloat("Grass end", &RenderSettings->GrassEnd, 0.0f, 1.0f))
	{
		RenderSettings->GrassEndCascade0 = std::min(RenderSettings->GrassEndCascade0, RenderSettings->GrassEnd);
		RenderSettings->GrassEndCascade1 = std::min(RenderSettings->GrassEndCascade1, RenderSettings->GrassEnd);
	}
	
	ImGui::End();

	HandleIncorrectValues();
}

void GrassWindow::HandleIncorrectValues() const
{
	RenderSettings->GrassGeneratorSurfaceWidth = std::max(EPSILON, RenderSettings->GrassGeneratorSurfaceWidth);
	RenderSettings->GrassGeneratorSurfaceHeight = std::max(EPSILON, RenderSettings->GrassGeneratorSurfaceHeight);
	RenderSettings->GrassGeneratorMaxCameraDistance = std::max(0.0f, RenderSettings->GrassGeneratorMaxCameraDistance);
}
