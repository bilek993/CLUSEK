#include "RoadComponentEditor.h"

#include "../../Utils/CameraLocator.h"
#include "../../Utils/GuiTransformUtil.h"

void RoadComponentEditor::Draw()
{
	const auto componentPointer = GetPointerToThisComponent();
	
	DrawPoints(componentPointer);
	ImGui::Separator();
	DrawControlButtons(componentPointer);

	DrawPointsWithGizmosOnScreen(componentPointer);
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

void RoadComponentEditor::DrawPointsWithGizmosOnScreen(RoadComponent* componentPointer) const
{
	const auto& io = ImGui::GetIO();
	
	const auto mainCamera = CameraLocator::GetMainCamera(Registry);
	const auto viewProjectionMatrix = mainCamera.ViewMatrix * mainCamera.ProjectionMatrix;
	
	for (auto& point : componentPointer->Points)
	{
		bool notVisible{};
		const auto circlePosition = GuiTransformUtil::TransformWorldPositionToScreenPoint(	point, 
																							viewProjectionMatrix, 
																							notVisible,
																							Config->MainCameraFarZ,
																							Config->MainCameraNearZ);
		if (notVisible)
			continue;

		const auto isCursorTouchingPoint = GuiTransformUtil::IsTouching(circlePosition, io.MousePos, DOT_SIZE);
		
		FullscreenDrawList->AddCircleFilled(circlePosition, DOT_SIZE, isCursorTouchingPoint ? DOT_COLOR_SECONDARY : DOT_COLOR_PRIMARY);
	}
}
