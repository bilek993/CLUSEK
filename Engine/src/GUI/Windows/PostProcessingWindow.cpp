#include "PostProcessingWindow.h"
#include <imgui.h>

void PostProcessingWindow::Draw()
{
	ImGui::Begin("Post Processing settings", &IsEnabled);

	//ImGui::Text("Post processing stack:");

	//ImGui::Checkbox("Tone Mapper", &CurrentPostProcessingSettings->ToneMapperEnabled);
	//ImGui::Checkbox("Gamma Correction", &CurrentPostProcessingSettings->GammaCorrectionEnabled);

	ImGui::End();
}
