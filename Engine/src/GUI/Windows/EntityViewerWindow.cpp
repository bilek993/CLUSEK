#include "EntityViewerWindow.h"
#include "../../ECS/Components/TransformComponent.h"
#include "../../ECS/Components/CameraComponent.h"

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
		ImGui::BulletText("Entity %i", counter++);

		DrawComponent<CameraComponent>(entity, "Camera Component");
	});
}
