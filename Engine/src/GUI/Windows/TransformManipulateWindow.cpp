#include "TransformManipulateWindow.h"
#include "../../ECS/Components/ModelRenderComponent.h"
#include "../../ECS/Components/SkyboxComponent.h"
#include "../../ECS/Components/TerrainComponent.h"
#include "../../ECS/Systems/RenderSystem.h"

void TransformManipulateWindow::Draw()
{
	ImGui::Begin("Transform manipulation", &IsEnabled);

	DrawCombo();
	DrawDetails();

	ImGui::End();
}

void TransformManipulateWindow::DrawCombo()
{
	std::string listIds{};
	auto counter = 0;

	ListComponent<ModelRenderComponent>(counter, listIds);
	ListComponent<SkyboxComponent>(counter, listIds);
	ListComponent<TerrainComponent>(counter, listIds);

	ImGui::Combo("Selected entity Id", &SelectedId, listIds.c_str());
}

void TransformManipulateWindow::DrawDetails()
{
	if (CurrentWorldMatrix == nullptr)
		return;

	const auto renderSystem = dynamic_cast<RenderSystem*>((*Systems)[RenderSystemId].System.get());
	const auto mainCamera = renderSystem->GetMainCamera();

	DirectX::XMFLOAT4X4 viewMatrixFloats{};
	XMStoreFloat4x4(&viewMatrixFloats, mainCamera.ViewMatrix);

	DirectX::XMFLOAT4X4 projectionMatrixFloats{};
	XMStoreFloat4x4(&projectionMatrixFloats, mainCamera.ProjectionMatrix);

	DirectX::XMFLOAT4X4 worldMatrixFloats{};
	XMStoreFloat4x4(&worldMatrixFloats, *CurrentWorldMatrix);

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

	*CurrentWorldMatrix = XMLoadFloat4x4(&worldMatrixFloats);
}
