#include "EntityViewerWindow.h"
#include <imgui.h>
#include "../../ECS/Components/TransformComponent.h"

void EntityViewerWindow::Draw()
{
	ImGui::Begin("Entity Viewer", &IsEnabled);

	ImGui::Text("Entities list:");
	ImGui::Separator();

	DrawList();

	ImGui::End();
}

void EntityViewerWindow::DrawList()
{
	auto counter = 0;

	Registry->each([this, &counter](auto entity)
	{
		ImGui::Text("Entity %i", counter++);
	});
}
