#include "TransformManipulateWindow.h"
#include <imgui.h>
#include <ImGuizmo.h>
#include "../../ECS/Components/ModelRenderComponent.h"
#include "../../ECS/Components/SkyboxComponent.h"
#include "../../ECS/Components/TerrainComponent.h"

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

void TransformManipulateWindow::DrawDetails() const
{
	if (CurrentWorldMatrix == nullptr)
		return;

	DirectX::XMFLOAT4X4 worldMatrixFloats{};
	XMStoreFloat4x4(&worldMatrixFloats, *CurrentWorldMatrix);

	float matrixTranslation[3], matrixRotation[3], matrixScale[3];
	ImGuizmo::DecomposeMatrixToComponents(&worldMatrixFloats._11, matrixTranslation, matrixRotation, matrixScale);

	ImGui::InputFloat3("Translation", matrixTranslation, 3);
	ImGui::InputFloat3("Rotation", matrixRotation, 3);

	ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, &worldMatrixFloats._11);
	*CurrentWorldMatrix = XMLoadFloat4x4(&worldMatrixFloats);
}
