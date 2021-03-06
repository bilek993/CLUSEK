#pragma once
#include <PxQueryReport.h>

#include "BaseComponentEditor.h"
#include "../../ECS/Components/CameraComponent.h"
#include "../../ECS/Components/RoadComponent.h"
#include "../../ECS/Components/TransformComponent.h"

class RoadComponentEditor final : public BaseComponentEditor<RoadComponent>
{
protected:
	void Draw() override;

private:
	void DrawSelectMessage() const;
	
	void DrawMeshVertices(RoadComponent* componentPointer);
	void DrawPoints(RoadComponent* componentPointer);
	void DrawAdditionalConfiguration(RoadComponent* componentPointer) const;
	void DrawControlButtons(RoadComponent* componentPointer);
	void DrawRebuildButtons(RoadComponent* componentPointer);
	void DrawFullRecalculateButton(RoadComponent* componentPointer);

	void DrawPoints(RoadComponent* componentPointer, const DirectX::XMMATRIX& viewProjectionMatrix);
	void DrawConnectionLines(RoadComponent* componentPointer, const TransformComponent& cameraTransform, const DirectX::XMMATRIX& viewProjectionMatrix) const;
	void DrawGizmos(RoadComponent* componentPointer, const DirectX::XMMATRIX& viewMatrix, const DirectX::XMMATRIX& projectionMatrix);

	void HandleClickInSelectCreationMode(const DirectX::XMMATRIX& viewMatrix, const DirectX::XMMATRIX& projectionMatrix, RoadComponent* componentPointer);
	void AddNewPoints(RoadComponent* componentPointer, const DirectX::XMFLOAT3& hitPosition) const;
	physx::PxRaycastBuffer RayCastPhysics(float maxDistance, const DirectX::XMMATRIX& viewMatrix, const DirectX::XMMATRIX& projectionMatrix) const;

	void Rebuild();

	void RecalculateAllControlPoints(RoadComponent* componentPointer) const;
	void RecalculateNewlyAddedPoints(RoadComponent* componentPointer) const;

	ImVec2 FixVectorOutsideCameraPlanesIfNeeded(const ImVec2& pointToBeFixed, const ImVec2& secondPoint, 
		const DirectX::XMFLOAT3& cameraPosition, const DirectX::XMFLOAT3& pointToBeFixedWorldPosition, bool outsideCameraPlanes) const;

	int SelectedPointId = -1;

	bool SelectCreationMode = false;
	
	bool IsMoved = false;

	bool RebuildOnMove = true;
	bool RebuildOnAddOrRemove = true;
	bool RebuildOnRecalculate = true;

	inline const static float DOT_SIZE = 7.0f;
	inline const static ImColor DOT_COLOR_PRIMARY = ImColor(255, 255, 255);
	inline const static ImColor DOT_COLOR_SECONDARY = ImColor(175, 175, 175);

	inline const static ImColor LINE_COLOR_PRIMARY = ImColor(255, 255, 255, 255);
	inline const static ImColor LINE_COLOR_SECONDARY = ImColor(255, 255, 255, 100);
};
