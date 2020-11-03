#include "RoadComponentEditor.h"

void RoadComponentEditor::Draw()
{
	const auto componentPointer = GetPointerToThisComponent();
	
	DrawPoints(componentPointer);
	ImGui::Separator();
	DrawControlButtons(componentPointer);
}

void RoadComponentEditor::DrawPoints(RoadComponent* componentPointer) const
{
	auto counter = 0;

	ImGui::Text("List of road points:");
	
	for (auto& point : componentPointer->Points)
	{
		if (ImGui::TreeNode(("Point " + std::to_string(counter++)).c_str()))
		{
			ImGui::InputFloat3("Position", &point.x, 3);
			ImGui::TreePop();
		}
	}
}

void RoadComponentEditor::DrawControlButtons(RoadComponent* componentPointer) const
{
	ImGui::Text("Control buttons:");

	if (ImGui::Button("Add new point"))
	{
		// TODO: Add implementation
	}

	ImGui::SameLine();
	if (ImGui::Button("Remove first"))
	{
		// TODO: Add implementation
	}

	ImGui::SameLine();
	if (ImGui::Button("Remove last"))
	{
		// TODO: Add implementation
	}
}
