#include "PostProcessingWindow.h"
#include <imgui.h>

void PostProcessingWindow::Draw()
{
	ImGui::Begin("Post Processing settings", &IsEnabled);

	ImGui::Text("Post processing stack:");

	for (auto& effect : CurrentPostProcessingSettings->List)
	{
		ImGui::Text(effect->GetName().c_str());
	}

	ImGui::End();
}
