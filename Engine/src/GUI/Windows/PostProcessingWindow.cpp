#include "PostProcessingWindow.h"
#include <imgui.h>

void PostProcessingWindow::Draw()
{
	ImGui::Begin("Post Processing settings", &IsEnabled);

	DrawStack();

	ImGui::End();
}

void PostProcessingWindow::DrawStack() const
{
	ImGui::Text("Post processing stack:");

	for (auto i = 0; i < CurrentPostProcessingSettings->List.size(); i++)
	{
		ImGui::PushID(i);

		auto buttonLabel = "Remove##PostProcessing_" + std::to_string(i);
		if (ImGui::SmallButton(buttonLabel.c_str()))
		{
			CurrentPostProcessingSettings->List.erase(CurrentPostProcessingSettings->List.begin() + i);
			ImGui::PopID();
			break;
		}
		ImGui::SameLine();
		ImGui::Text(CurrentPostProcessingSettings->List[i]->GetName().c_str());

		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
		{
			ImGui::SetDragDropPayload(PAYLOAD_CELL_NAME.c_str(), &i, sizeof(int));
			ImGui::Text("Swap %s", CurrentPostProcessingSettings->List[i]->GetName().c_str());
			ImGui::EndDragDropSource();
		}
		if (ImGui::BeginDragDropTarget())
		{
			if (const auto payload = ImGui::AcceptDragDropPayload(PAYLOAD_CELL_NAME.c_str()))
			{
				const auto payloadId = *static_cast<const int*>(payload->Data);
				std::swap(CurrentPostProcessingSettings->List[i], CurrentPostProcessingSettings->List[payloadId]);
			}
			ImGui::EndDragDropTarget();
		}

		ImGui::PopID();
	}
}
