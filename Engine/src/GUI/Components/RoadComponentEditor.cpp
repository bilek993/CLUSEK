#include "RoadComponentEditor.h"
#include <ImGuizmo.h>
#include <json.hpp>
#include "../../Utils/CameraLocator.h"
#include "../../Utils/GuiTransformUtil.h"

void RoadComponentEditor::Draw()
{
	const auto componentPointer = GetPointerToThisComponent();

	const auto mainCamera = CameraLocator::GetMainCamera(Registry);
	const auto viewProjectionMatrix = mainCamera.ViewMatrix * mainCamera.ProjectionMatrix;
	
	DrawPoints(componentPointer);
	ImGui::Separator();
	DrawControlButtons(componentPointer);

	DrawPoints(componentPointer, viewProjectionMatrix);
	DrawConnectionLines(componentPointer, viewProjectionMatrix);
	DrawGizmos(componentPointer, mainCamera.ViewMatrix, mainCamera.ProjectionMatrix);
}

void RoadComponentEditor::DrawPoints(RoadComponent* componentPointer)
{
	auto counter = 0;

	ImGui::Text("List of road points:");
	
	for (auto& point : componentPointer->Points)
	{
		if (ImGui::TreeNode(("Point " + std::to_string(counter)).c_str()))
		{
			ImGui::InputFloat3("Position", &point.x, 3);

			if (ImGui::Button("Select"))
				SelectedPointId = counter;
			
			ImGui::TreePop();
		}

		counter++;
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

	if (ImGui::Button("Copy JSON"))
	{
		std::vector<nlohmann::json> vectorOfPoints{};
		for (auto& point : componentPointer->Points)
		{
			nlohmann::json newJsonPointObject{
				{"PositionX", point.x},
				{"PositionY", point.y},
				{"PositionZ", point.z}
			};

			vectorOfPoints.emplace_back(newJsonPointObject);
		}

		nlohmann::json outputJsonObject{};
		outputJsonObject["Points"] = vectorOfPoints;

		ImGui::LogToClipboard();
		ImGui::LogText(outputJsonObject.dump().c_str());
		ImGui::LogFinish();
	}
}

void RoadComponentEditor::DrawPoints(RoadComponent* componentPointer, const DirectX::XMMATRIX& viewProjectionMatrix)
{
	const auto& io = ImGui::GetIO();

	auto counter = 0;
	
	for (auto& point : componentPointer->Points)
	{
		bool visible{};
		const auto circlePosition = GuiTransformUtil::TransformWorldPositionToScreenPoint(	point, 
																							viewProjectionMatrix, 
																							visible,
																							Config->MainCameraFarZ,
																							Config->MainCameraNearZ);
		if (visible)
		{
			const auto isCursorTouchingPoint = GuiTransformUtil::IsTouching(circlePosition, io.MousePos, DOT_SIZE);

			if (isCursorTouchingPoint && ImGui::IsMouseClicked(0) && !ImGui::IsAnyItemHovered() && !ImGui::IsAnyItemActive())
				SelectedPointId = counter;

			FullscreenDrawList->AddCircleFilled(circlePosition, DOT_SIZE, isCursorTouchingPoint ? DOT_COLOR_SECONDARY : DOT_COLOR_PRIMARY);
		}
		
		counter++;
	}
}

void RoadComponentEditor::DrawConnectionLines(RoadComponent* componentPointer, const DirectX::XMMATRIX& viewProjectionMatrix) const
{
	for (auto curElementId = 0; curElementId < componentPointer->Points.size(); curElementId += 3)
	{
		const auto prevElementId = curElementId - 1;
		const auto nextElementId = curElementId + 1;

		const auto isPrevPointSelected = SelectedPointId == prevElementId;
		const auto isCurrentPointSelected = SelectedPointId == curElementId;
		const auto isNextPointSelected = SelectedPointId == nextElementId;

		bool curPointVisible{};
		const auto curPointPosition = GuiTransformUtil::TransformWorldPositionToScreenPoint(componentPointer->Points[curElementId],
																							viewProjectionMatrix, 
																							curPointVisible,
																							Config->MainCameraFarZ,
																							Config->MainCameraNearZ);

		if (curPointVisible)
		{
			if (prevElementId > 0)
			{
				bool prevPointVisible{};
				const auto prevPointPosition = GuiTransformUtil::TransformWorldPositionToScreenPoint(	componentPointer->Points[prevElementId],
																										viewProjectionMatrix, 
																										prevPointVisible,
																										Config->MainCameraFarZ,
																										Config->MainCameraNearZ);

				if (prevPointVisible)
					FullscreenDrawList->AddLine(curPointPosition, prevPointPosition, isCurrentPointSelected || isPrevPointSelected ? LINE_COLOR_PRIMARY : LINE_COLOR_SECONDARY);
			}

			if (nextElementId < componentPointer->Points.size())
			{
				bool nextPointVisible{};
				const auto nextPointPosition = GuiTransformUtil::TransformWorldPositionToScreenPoint(	componentPointer->Points[nextElementId],
																										viewProjectionMatrix, 
																										nextPointVisible,
																										Config->MainCameraFarZ,
																										Config->MainCameraNearZ);

				if (nextPointVisible)
					FullscreenDrawList->AddLine(curPointPosition, nextPointPosition, isCurrentPointSelected || isNextPointSelected ? LINE_COLOR_PRIMARY : LINE_COLOR_SECONDARY);
			}
		}
	}
}

void RoadComponentEditor::DrawGizmos(RoadComponent* componentPointer, const DirectX::XMMATRIX& viewMatrix, const DirectX::XMMATRIX& projectionMatrix) const
{
	if (SelectedPointId < 0 || SelectedPointId >= componentPointer->Points.size())
		return;

	auto worldMatrix = DirectX::XMMatrixTranslation(componentPointer->Points[SelectedPointId].x, 
													componentPointer->Points[SelectedPointId].y, 
													componentPointer->Points[SelectedPointId].z);

	DirectX::XMFLOAT4X4 viewMatrixFloats{};
	XMStoreFloat4x4(&viewMatrixFloats, viewMatrix);

	DirectX::XMFLOAT4X4 projectionMatrixFloats{};
	XMStoreFloat4x4(&projectionMatrixFloats, projectionMatrix);

	DirectX::XMFLOAT4X4 worldMatrixFloats{};
	XMStoreFloat4x4(&worldMatrixFloats, worldMatrix);

	auto& io = ImGui::GetIO();
	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
	Manipulate(&viewMatrixFloats._11, &projectionMatrixFloats._11, ImGuizmo::TRANSLATE, ImGuizmo::WORLD, &worldMatrixFloats._11);

	worldMatrix = XMLoadFloat4x4(&worldMatrixFloats);

	DirectX::XMVECTOR newScaleVector{};
	DirectX::XMVECTOR newRotationVector{};
	DirectX::XMVECTOR newTranslationVector{};
	
	XMMatrixDecompose(&newScaleVector, &newRotationVector, &newTranslationVector, worldMatrix);

	XMStoreFloat3(&componentPointer->Points[SelectedPointId], newTranslationVector);
}
