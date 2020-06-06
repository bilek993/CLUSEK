#pragma once
#include "BaseComponentEditor.h"
#include <imgui.h>
#include <ImGuizmo.h>
#include "../../ECS/Components/TransformComponent.h"
#include "../../ECS/Components/CameraComponent.h"

class TransformComponentEditor final : public BaseComponentEditor<TransformComponent>
{
protected:
	void Draw() override;

private:
	ImGuizmo::OPERATION CurrentGizmoOperation = ImGuizmo::TRANSLATE;

	CameraComponent& GetMainCamera() const;
	TransformComponent& GetMainCameraTransform() const;
};
