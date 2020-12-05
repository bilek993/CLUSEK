#include "RoadComponentEditor.h"
#include <ImGuizmo.h>
#include <json.hpp>
#include "../../Tags.h"
#include "../../ECS/Systems/PhysicsSystem.h"
#include "../../Renderer/TransformLogic.h"
#include "../../Utils/CameraLocator.h"
#include "../../Utils/GuiTransformUtil.h"
#include "../../Utils/RayUtil.h"
#include "../../Utils/SplineUtil.h"

void RoadComponentEditor::Draw()
{
	const auto componentPointer = GetPointerToThisComponent();

	const auto mainCameraTransform = CameraLocator::GetMainCameraTransform(Registry);

	const auto mainCamera = CameraLocator::GetMainCamera(Registry);
	const auto viewProjectionMatrix = mainCamera.ViewMatrix * mainCamera.ProjectionMatrix;

	if (SelectCreationMode)
	{
		DrawSelectMessage();
		HandleClickInSelectCreationMode(mainCamera.ViewMatrix, mainCamera.ProjectionMatrix, componentPointer);
	}
	else
	{
		DrawMeshVertices(componentPointer);
		ImGui::Separator();
		DrawPoints(componentPointer);
		ImGui::Separator();
		DrawAdditionalConfiguration(componentPointer);
		ImGui::Separator();
		DrawControlButtons(componentPointer);
		ImGui::Separator();
		DrawRebuildButtons(componentPointer);
		ImGui::Separator();
		DrawFullRecalculateButton(componentPointer);

		DrawPoints(componentPointer, viewProjectionMatrix);
		DrawConnectionLines(componentPointer, mainCameraTransform, viewProjectionMatrix);
		DrawGizmos(componentPointer, mainCamera.ViewMatrix, mainCamera.ProjectionMatrix);
	}
}

void RoadComponentEditor::DrawSelectMessage() const
{
	ImGui::SetCursorPosX((ImGui::GetWindowSize().x / 2) - 95);
	ImGui::SetCursorPosY((ImGui::GetWindowSize().y / 2));
	ImGui::Text("Please select point on the map...");
}

void RoadComponentEditor::DrawMeshVertices(RoadComponent* componentPointer)
{
	ImGui::Text("Mesh vertices (%u vertices):", static_cast<unsigned>(componentPointer->MeshVertices.size()));

	for (auto& vertex : componentPointer->MeshVertices)
		ImGui::BulletText("X: %f Y: %f", vertex.x, vertex.y);
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

			if (SelectedPointId == counter)
			{
				if (ImGui::Button("Unselect"))
					SelectedPointId = -1;
			}
			else
			{
				if (ImGui::Button("Select"))
					SelectedPointId = counter;
			}
			
			ImGui::TreePop();
		}

		counter++;
	}
}

void RoadComponentEditor::DrawAdditionalConfiguration(RoadComponent* componentPointer) const
{
	ImGui::DragInt("Resolution", &componentPointer->Resolution, 10);
	ImGui::DragFloat("Split distance", &componentPointer->SplitDistance, 0.1f);
	ImGui::DragFloat("Texture scaling", &componentPointer->TextureScaling, 1.0f);
}

void RoadComponentEditor::DrawControlButtons(RoadComponent* componentPointer)
{
	ImGui::Text("Control buttons:");

	if (ImGui::Button("Add new point"))
	{
		Logger::Debug("Enabled selection mode for adding new road point...");
		SelectCreationMode = true;
	}

	ImGui::SameLine();
	if (ImGui::Button("Remove first"))
	{
		Logger::Debug("Removing first road point...");
		
		if (componentPointer->Points.size() > 5)
			componentPointer->Points.erase(componentPointer->Points.begin(), componentPointer->Points.cbegin() + 3);
		else if (componentPointer->Points.size() == 4)
			componentPointer->Points.erase(componentPointer->Points.begin(), componentPointer->Points.cbegin() + 2);
		else
			componentPointer->Points.clear();
		
		if (RebuildOnAddOrRemove)
		{
			Logger::Debug("Rebuilding due to removing point!");
			Rebuild();
		}
	}

	ImGui::SameLine();
	if (ImGui::Button("Remove last"))
	{
		Logger::Debug("Removing last road point...");

		if (componentPointer->Points.size() > 5)
			componentPointer->Points.erase(componentPointer->Points.cend() - 3, componentPointer->Points.cend());
		else if (componentPointer->Points.size() == 4)
			componentPointer->Points.erase(componentPointer->Points.cend() - 2, componentPointer->Points.cend());
		else
			componentPointer->Points.clear();
		
		if (RebuildOnAddOrRemove)
		{
			Logger::Debug("Rebuilding due to removing point!");
			Rebuild();
		}
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

void RoadComponentEditor::DrawRebuildButtons(RoadComponent* componentPointer)
{
	ImGui::Text("Rebuild configuration:");
	if (ImGui::Button("Force rebuild"))
	{
		Logger::Debug("Forcing rebuild now!");
		Rebuild();
	}

	ImGui::Checkbox("Rebuild on move", &RebuildOnMove);
	ImGui::Checkbox("Rebuild on add or remove", &RebuildOnAddOrRemove);
	ImGui::Checkbox("Rebuild on recalculate", &RebuildOnRecalculate);
}

void RoadComponentEditor::DrawFullRecalculateButton(RoadComponent* componentPointer)
{
	ImGui::Text("Recalculate all control points:");
	if (ImGui::Button("Recalculate now"))
	{
		Logger::Debug("Forcing all control points recalculation!");

		if (componentPointer->Points.size() < 5)
		{
			Logger::Warning("Full recalculation is not available for less that 5 points!");
			return;
		}
		
		RecalculateAllControlPoints(componentPointer);

		if (RebuildOnRecalculate)
			Rebuild();
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
																									&visible,
																									nullptr,
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

void RoadComponentEditor::DrawConnectionLines(RoadComponent* componentPointer, const TransformComponent& cameraTransform, const DirectX::XMMATRIX& viewProjectionMatrix) const
{
	for (auto curElementId = 0; curElementId < componentPointer->Points.size(); curElementId += 3)
	{
		const auto prevElementId = curElementId - 1;
		const auto nextElementId = curElementId + 1;

		const auto isPrevPointSelected = SelectedPointId == prevElementId;
		const auto isCurrentPointSelected = SelectedPointId == curElementId;
		const auto isNextPointSelected = SelectedPointId == nextElementId;

		bool curPointOutsideCameraPlanes{};
		const auto curPointPosition = GuiTransformUtil::TransformWorldPositionToScreenPoint(	componentPointer->Points[curElementId],
																										viewProjectionMatrix, 
																										nullptr,
																										&curPointOutsideCameraPlanes,
																										Config->MainCameraFarZ,
																										Config->MainCameraNearZ);

		if (prevElementId > 0)
		{
			bool prevPointOutsideCameraPlanes{};
			const auto prevPointPosition = GuiTransformUtil::TransformWorldPositionToScreenPoint(	componentPointer->Points[prevElementId],
																											viewProjectionMatrix, 
																											nullptr,
																											&prevPointOutsideCameraPlanes,
																											Config->MainCameraFarZ,
																											Config->MainCameraNearZ);

			if (!prevPointOutsideCameraPlanes || !curPointOutsideCameraPlanes)
			{
				const auto fixedCurPointPosition = FixVectorOutsideCameraPlanesIfNeeded(	curPointPosition, 
																									prevPointPosition, 
																									TransformLogic::GetPosition(cameraTransform), 
																									componentPointer->Points[curElementId], 
																									curPointOutsideCameraPlanes);
				
				const auto fixedPrevPointPosition = FixVectorOutsideCameraPlanesIfNeeded(	prevPointPosition, 
																									curPointPosition, 
																									TransformLogic::GetPosition(cameraTransform), 
																									componentPointer->Points[prevElementId], 
																									prevPointOutsideCameraPlanes);
				
				FullscreenDrawList->AddLine(fixedCurPointPosition, fixedPrevPointPosition, isCurrentPointSelected || isPrevPointSelected ? LINE_COLOR_PRIMARY : LINE_COLOR_SECONDARY);
			}
		}

		if (nextElementId < componentPointer->Points.size())
		{
			bool nextPointOutsideCameraPlanes{};
			const auto nextPointPosition = GuiTransformUtil::TransformWorldPositionToScreenPoint(	componentPointer->Points[nextElementId],
																											viewProjectionMatrix, 
																											nullptr,
																											&nextPointOutsideCameraPlanes,
																											Config->MainCameraFarZ,
																											Config->MainCameraNearZ);

			if (!nextPointOutsideCameraPlanes || !curPointOutsideCameraPlanes)
			{
				const auto fixedCurPointPosition = FixVectorOutsideCameraPlanesIfNeeded(	curPointPosition, 
																									nextPointPosition, 
																									TransformLogic::GetPosition(cameraTransform), 
																									componentPointer->Points[curElementId], 
																									curPointOutsideCameraPlanes);

				const auto fixedNextPointPosition = FixVectorOutsideCameraPlanesIfNeeded(	nextPointPosition, 
																									curPointPosition, 
																									TransformLogic::GetPosition(cameraTransform), 
																									componentPointer->Points[nextElementId], 
																									nextPointOutsideCameraPlanes);

				FullscreenDrawList->AddLine(fixedCurPointPosition, fixedNextPointPosition, isCurrentPointSelected || isNextPointSelected ? LINE_COLOR_PRIMARY : LINE_COLOR_SECONDARY);
			}
		}
	}
}

void RoadComponentEditor::DrawGizmos(RoadComponent* componentPointer, const DirectX::XMMATRIX& viewMatrix, const DirectX::XMMATRIX& projectionMatrix)
{
	if (SelectedPointId < 0 || SelectedPointId >= componentPointer->Points.size())
		return;

	auto worldMatrix = DirectX::XMMatrixTranslation(	componentPointer->Points[SelectedPointId].x, 
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

	if (ImGuizmo::IsUsing() && componentPointer->Points.size() >= 4)
	{
		const auto oldTranslationVector = XMLoadFloat3(&componentPointer->Points[SelectedPointId]);
		const auto translationDifference = DirectX::XMVectorSubtract(newTranslationVector, oldTranslationVector);
		
		if (SelectedPointId % 3 == 0)
		{
			if (SelectedPointId != componentPointer->Points.size() - 1)
			{
				auto controlPoint = XMLoadFloat3(&componentPointer->Points[SelectedPointId + 1]);
				controlPoint = DirectX::XMVectorAdd(controlPoint, translationDifference);
				XMStoreFloat3(&componentPointer->Points[SelectedPointId + 1], controlPoint);
			}

			if (SelectedPointId != 0)
			{
				auto controlPoint = XMLoadFloat3(&componentPointer->Points[SelectedPointId - 1]);
				controlPoint = DirectX::XMVectorAdd(controlPoint, translationDifference);
				XMStoreFloat3(&componentPointer->Points[SelectedPointId - 1], controlPoint);
			}
		}
	}
	
	XMStoreFloat3(&componentPointer->Points[SelectedPointId], newTranslationVector);

	if (IsMoved && !ImGuizmo::IsUsing())
	{
		IsMoved = false;
		
		Logger::Debug("Rebuilding due to point move!");
		Rebuild();
	}
	else if (!IsMoved && ImGuizmo::IsUsing())
	{
		IsMoved = true;
	}
}

void RoadComponentEditor::HandleClickInSelectCreationMode(const DirectX::XMMATRIX& viewMatrix, const DirectX::XMMATRIX& projectionMatrix, RoadComponent* componentPointer)
{
	if (IoData->MouseTracker.leftButton != DirectX::Mouse::ButtonStateTracker::ButtonState::PRESSED)
		return;

	if (IoData->MouseState.positionMode == DirectX::Mouse::MODE_RELATIVE)
	{
		Logger::Debug("Skipping handling mouse for UI due to relative mouse mode!");
		return;
	}
	
	const auto hit = RayCastPhysics(100000.0f, viewMatrix, projectionMatrix);
	if (!hit.hasBlock)
	{
		Logger::Debug("Ray didn't hit any object.");
		return;
	}

	AddNewPoints(componentPointer, DirectX::XMFLOAT3(hit.block.position.x, hit.block.position.y, hit.block.position.z));
	RecalculateNewlyAddedPoints(componentPointer);
	
	SelectCreationMode = false;
	
	if (RebuildOnAddOrRemove)
	{
		Logger::Debug("Rebuilding due to adding new point!");
		Rebuild();
	}
}

void RoadComponentEditor::AddNewPoints(RoadComponent* componentPointer, const DirectX::XMFLOAT3& hitPosition) const
{
	if (componentPointer->Points.empty())
	{
		componentPointer->Points.emplace_back(DirectX::XMFLOAT3(hitPosition.x + 10.0f, hitPosition.y, hitPosition.z + 10.0f));
		componentPointer->Points.emplace_back(hitPosition);
	}
	else if (componentPointer->Points.size() == 2)
	{
		componentPointer->Points.emplace_back();
		componentPointer->Points.emplace_back(hitPosition);
	}
	else
	{
		componentPointer->Points.emplace_back();
		componentPointer->Points.emplace_back();
		componentPointer->Points.emplace_back(hitPosition);
	}
}

physx::PxRaycastBuffer RoadComponentEditor::RayCastPhysics(const float maxDistance, const DirectX::XMMATRIX& viewMatrix, const DirectX::XMMATRIX& projectionMatrix) const
{
	DirectX::XMVECTOR rayOriginVector{};
	DirectX::XMVECTOR rayDirectionVector{};
	
	RayUtil::MousePositionToRayOriginAndDirection(	IoData->MouseState.x,
													IoData->MouseState.y,
													Config->WindowWidth,
													Config->WindowHeight,
													viewMatrix,
													projectionMatrix,
													rayOriginVector,
													rayDirectionVector);

	DirectX::XMFLOAT3 rayOrigin{};
	DirectX::XMFLOAT3 rayDirection{};
	
	XMStoreFloat3(&rayOrigin, rayOriginVector);
	XMStoreFloat3(&rayDirection, rayDirectionVector);

	const auto origin = physx::PxVec3(rayOrigin.x, rayOrigin.y, rayOrigin.z);
	const auto unitDir = physx::PxVec3(rayDirection.x, rayDirection.y, rayDirection.z);
	physx::PxRaycastBuffer hit;
	
	const auto physicsSystem = dynamic_cast<PhysicsSystem*>((*Systems)[PhysicsSystemId].System.get());
	
	const auto scene = physicsSystem->GetPointerToScene();
	scene->raycast(origin, unitDir, maxDistance, hit);

	return hit;
}

void RoadComponentEditor::Rebuild()
{
	Registry->assign<entt::tag<Tags::REQUIRES_REBUILD>>(*SelectedEntity);
	
	for (auto &system : *Systems)
		system.System->Rebuild();
}

void RoadComponentEditor::RecalculateAllControlPoints(RoadComponent* componentPointer) const
{
	// Middle points recalculation
	
	for (auto i = 3; i <= componentPointer->Points.size() - 4; i += 3)
	{
		const auto currentAnchorPoint = XMLoadFloat3(&componentPointer->Points[i]);
		const auto previousAnchorPoint = XMLoadFloat3(&componentPointer->Points[i - 3]);
		const auto nextAnchorPoint = XMLoadFloat3(&componentPointer->Points[i + 3]);

		auto previousControlPoint = XMLoadFloat3(&componentPointer->Points[i - 1]);
		auto nextControlPoint = XMLoadFloat3(&componentPointer->Points[i + 1]);
		
		SplineUtil::RecalculateMiddleControlPoint(currentAnchorPoint, previousAnchorPoint, nextAnchorPoint, &previousControlPoint, &nextControlPoint);

		XMStoreFloat3(&componentPointer->Points[i - 1], previousControlPoint);
		XMStoreFloat3(&componentPointer->Points[i + 1], nextControlPoint);
	}

	// First point recalculation

	const auto firstPoint = XMLoadFloat3(&componentPointer->Points[0]);
	auto secondPoint = XMLoadFloat3(&componentPointer->Points[1]);
	const auto thirdPoint = XMLoadFloat3(&componentPointer->Points[2]);
	
	SplineUtil::RecalculateFirstControlPoint(firstPoint, secondPoint, thirdPoint);

	XMStoreFloat3(&componentPointer->Points[1], secondPoint);

	// Last point recalculation

	const auto lastPoint = XMLoadFloat3(&componentPointer->Points[componentPointer->Points.size() - 1]);
	auto secondLastPoint = XMLoadFloat3(&componentPointer->Points[componentPointer->Points.size() - 2]);
	const auto thirdLastPoint = XMLoadFloat3(&componentPointer->Points[componentPointer->Points.size() - 3]);

	SplineUtil::RecalculateLastControlPoint(lastPoint, secondLastPoint, thirdLastPoint);

	XMStoreFloat3(&componentPointer->Points[componentPointer->Points.size() - 2], secondLastPoint);
}

void RoadComponentEditor::RecalculateNewlyAddedPoints(RoadComponent* componentPointer) const
{
	if (componentPointer->Points.size() == 4)
	{
		// First point recalculation
		
		const auto firstPoint = XMLoadFloat3(&componentPointer->Points[0]);
		auto secondPoint = XMLoadFloat3(&componentPointer->Points[1]);
		const auto thirdPoint = XMLoadFloat3(&componentPointer->Points[2]);

		SplineUtil::RecalculateFirstControlPoint(firstPoint, secondPoint, thirdPoint);

		XMStoreFloat3(&componentPointer->Points[1], secondPoint);

		// Last point recalculation

		const auto lastPoint = XMLoadFloat3(&componentPointer->Points[componentPointer->Points.size() - 1]);
		auto secondLastPoint = XMLoadFloat3(&componentPointer->Points[componentPointer->Points.size() - 2]);
		const auto thirdLastPoint = XMLoadFloat3(&componentPointer->Points[componentPointer->Points.size() - 3]);

		SplineUtil::RecalculateFirstControlPoint(lastPoint, secondLastPoint, thirdLastPoint);

		XMStoreFloat3(&componentPointer->Points[componentPointer->Points.size() - 2], secondLastPoint);
	}
	else if (componentPointer->Points.size() >= 7)
	{
		// One before last point recalculation

		const auto previousAnchorId = componentPointer->Points.size() - 4;

		const auto anchorPoint0 = XMLoadFloat3(&componentPointer->Points[previousAnchorId]);
		const auto anchorPoint1 = XMLoadFloat3(&componentPointer->Points[previousAnchorId - 3]);
		const auto anchorPoint2 = XMLoadFloat3(&componentPointer->Points[previousAnchorId + 3]);

		auto controlPoint0 = XMLoadFloat3(&componentPointer->Points[previousAnchorId - 1]);
		auto controlPoint1 = XMLoadFloat3(&componentPointer->Points[previousAnchorId + 1]);

		SplineUtil::RecalculateMiddleControlPoint(	anchorPoint0, 
													anchorPoint1, 
													anchorPoint2, 
													&controlPoint0,
													&controlPoint1);

		XMStoreFloat3(&componentPointer->Points[previousAnchorId - 1], controlPoint0);
		XMStoreFloat3(&componentPointer->Points[previousAnchorId + 1], controlPoint1);
		
		// Last point recalculation

		const auto lastPoint = XMLoadFloat3(&componentPointer->Points[componentPointer->Points.size() - 1]);
		auto secondLastPoint = XMLoadFloat3(&componentPointer->Points[componentPointer->Points.size() - 2]);
		const auto thirdLastPoint = XMLoadFloat3(&componentPointer->Points[componentPointer->Points.size() - 3]);

		SplineUtil::RecalculateFirstControlPoint(lastPoint, secondLastPoint, thirdLastPoint);

		XMStoreFloat3(&componentPointer->Points[componentPointer->Points.size() - 2], secondLastPoint);
	}
	else
	{
		Logger::Debug("Recalculating not possible!");
	}
}

ImVec2 RoadComponentEditor::FixVectorOutsideCameraPlanesIfNeeded(const ImVec2& pointToBeFixed, const ImVec2& secondPoint, 
                                                                 const DirectX::XMFLOAT3& cameraPosition, const DirectX::XMFLOAT3& pointToBeFixedWorldPosition, const bool outsideCameraPlanes) const
{
	if (!outsideCameraPlanes)
		return pointToBeFixed;
	
	// Using linear equation to solve this problem
	// y = ax + b

	const auto a = (secondPoint.y - pointToBeFixed.y) / (secondPoint.x - pointToBeFixed.x);
	const auto b = pointToBeFixed.y - (a * pointToBeFixed.x);

	auto& io = ImGui::GetIO();
	const auto height = io.DisplaySize.y;

	if (cameraPosition.y - pointToBeFixedWorldPosition.y > 0)
		return ImVec2((height - b) / a, height);
	else
		return ImVec2(-b / a, 0.0f);
}
