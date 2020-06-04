#include "EntityEditorWindow.h"
#include "../../ECS/Components/TransformComponent.h"

void EntityEditorWindow::Draw()
{
	ImGui::Begin("Entity Editor", &IsEnabled);

	if (*EntitySelected)
		DrawComponentsForEntity();
	else
		DrawNoElements();

	ImGui::End();
}

void EntityEditorWindow::DrawComponentsForEntity()
{
	REGISTER_COMPONENT_EDITOR(TransformComponent, "Transform Component");
}

void EntityEditorWindow::DrawNoElements() const
{
	ImGui::SetCursorPosX((ImGui::GetWindowSize().x / 2.0f) - 117.0f);
	ImGui::Text("Please select entity to enable this editor.");
}
