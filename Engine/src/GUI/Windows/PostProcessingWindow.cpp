#include "PostProcessingWindow.h"
#include <imgui.h>
#include "../../ECS/Systems/RenderSystem.h"

void PostProcessingWindow::Draw()
{
	ImGui::Begin("Post Processing settings", &IsEnabled);

	DrawStack();
	ImGui::Separator();
	DrawCreator();

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
			ImGui::Text("Swap this with %s", CurrentPostProcessingSettings->List[i]->GetName().c_str());
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

void PostProcessingWindow::DrawCreator()
{
	ImGui::Text("Add new post processing effect:");
	ImGui::Combo("", &SelectedItemForCreation, AVAILABLE_POST_PROCESSING_EFFECTS, IM_ARRAYSIZE(AVAILABLE_POST_PROCESSING_EFFECTS));
	ImGui::SameLine();
	if (ImGui::Button("Add to stack"))
		AddNewPostProcessingEffect();
}

void PostProcessingWindow::AddNewPostProcessingEffect() const
{
	const auto renderSystem = dynamic_cast<RenderSystem*>((*Systems)[RenderSystemId].System.get());
	const auto windowWidth = Config->WindowWidth;
	const auto windowHeight = Config->WindowHeight;

	switch (SelectedItemForCreation)
	{
	case 0:
		MAP_POST_PROCESSING_EFFECT("Gamma Correction", "gamma_correction_pixel_shader.cso");
		break;
	case 1:
		MAP_POST_PROCESSING_EFFECT("Reinhard Tone Mapper", "reinhard_tone_mapper_pixel_shader.cso");
		break;
	case 2:
		MAP_POST_PROCESSING_EFFECT("Simple ACES Tone Mapper", "simple_aces_tone_mapper_pixel_shader.cso");
		break;
	case 3:
		MAP_POST_PROCESSING_EFFECT("Complex ACES Tone Mapper", "complex_aces_tone_mapper_pixel_shader.cso");
		break;
	case 4:
		MAP_POST_PROCESSING_EFFECT("Chromatic Aberration", "chromatic_aberration_pixel_shader.cso");
		break;
	default:
		Logger::Warning("Selected item is incorrect.");
	}
}
