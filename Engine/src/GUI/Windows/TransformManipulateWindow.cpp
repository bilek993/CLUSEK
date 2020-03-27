#include "TransformManipulateWindow.h"
#include <imgui.h>
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

void TransformManipulateWindow::DrawDetails()
{
}
