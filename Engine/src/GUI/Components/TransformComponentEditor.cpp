#include "TransformComponentEditor.h"
#include "../../Tags.h"
#include "../../ECS/Systems/RenderSystem.h"

void TransformComponentEditor::Draw()
{
	const auto renderSystem = dynamic_cast<RenderSystem*>((*Systems)[RenderSystemId].System.get());
	const auto mainCamera = renderSystem->GetMainCamera();

	DirectX::XMFLOAT4X4 viewMatrixFloats{};
	XMStoreFloat4x4(&viewMatrixFloats, mainCamera.ViewMatrix);

	DirectX::XMFLOAT4X4 projectionMatrixFloats{};
	XMStoreFloat4x4(&projectionMatrixFloats, mainCamera.ProjectionMatrix);

	DirectX::XMFLOAT4X4 worldMatrixFloats{};
	XMStoreFloat4x4(&worldMatrixFloats, GetPointerToThisComponent()->WorldMatrix);

	float matrixTranslation[3], matrixRotation[3], matrixScale[3];
	ImGuizmo::DecomposeMatrixToComponents(&worldMatrixFloats._11, matrixTranslation, matrixRotation, matrixScale);

	ImGui::InputFloat3("Translation", matrixTranslation, 3);
	ImGui::InputFloat3("Rotation", matrixRotation, 3);

	ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, &worldMatrixFloats._11);

	if (ImGui::RadioButton("Translate", CurrentGizmoOperation == ImGuizmo::TRANSLATE))
		CurrentGizmoOperation = ImGuizmo::TRANSLATE;
	ImGui::SameLine();
	if (ImGui::RadioButton("Rotate", CurrentGizmoOperation == ImGuizmo::ROTATE))
		CurrentGizmoOperation = ImGuizmo::ROTATE;

	auto& io = ImGui::GetIO();
	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
	Manipulate(&viewMatrixFloats._11, &projectionMatrixFloats._11, CurrentGizmoOperation, ImGuizmo::WORLD, &worldMatrixFloats._11);

	GetPointerToThisComponent()->WorldMatrix = XMLoadFloat4x4(&worldMatrixFloats);
}

CameraComponent& TransformComponentEditor::GetMainCamera() const
{
	auto view = Registry->view<CameraComponent, entt::tag<Tags::MAIN_CAMERA>>();
	if (view.size() != 1)
	{
		if (view.size() > 1)
			Logger::Error("More than one main render camera found!");
		else
			Logger::Error("Main render camera not found!");
	}

	return view.raw<CameraComponent>()[0];
}

TransformComponent& TransformComponentEditor::GetMainCameraTransform() const
{
	auto view = Registry->view<CameraComponent, TransformComponent, entt::tag<Tags::MAIN_CAMERA>>();
	if (view.size() != 1)
	{
		if (view.size() > 1)
			Logger::Error("More than one main render camera found!");
		else
			Logger::Error("Main render camera not found!");
	}

	return view.raw<TransformComponent>()[0];
}
