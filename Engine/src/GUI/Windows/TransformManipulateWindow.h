#pragma once
#include "BaseWindow.h"
#include <imgui.h>
#include <ImGuizmo.h>
#include "../../ECS/Components/CameraComponent.h"
#include "../../ECS/Components/TransformComponent.h"

class TransformManipulateWindow final : public BaseWindow
{
protected:
	void Draw() override;
private:
	void DrawCombo();
	void DrawDetails();

	void ListComponent(std::string& list);

	void HandleClicking();
	CameraComponent& GetMainCamera() const;
	TransformComponent& GetMainCameraTransform() const;

	ImGuizmo::OPERATION CurrentGizmoOperation = ImGuizmo::TRANSLATE;

	int SelectedId = -1;
	bool ClickSelectEnabled = false;
	DirectX::XMMATRIX* CurrentWorldMatrix = nullptr;
};
