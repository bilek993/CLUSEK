#include "PostProcessingWindow.h"
#include <imgui.h>

void PostProcessingWindow::Draw()
{
	ImGui::Begin("Post Processing settings", &IsEnabled);
	ImGui::Text("Post processing stack:");

	for (auto i = 0; i < CurrentPostProcessingSettings->List.size(); i++)
	{
		auto buttonLabel = "Remove##PostProcessing_" + std::to_string(i);
		if (ImGui::SmallButton(buttonLabel.c_str()))
		{
			CurrentPostProcessingSettings->List.erase(CurrentPostProcessingSettings->List.begin() + i);
			break;
		}
		ImGui::SameLine();
		ImGui::Text(CurrentPostProcessingSettings->List[i]->GetName().c_str());
	}

	ImGui::End();
}
