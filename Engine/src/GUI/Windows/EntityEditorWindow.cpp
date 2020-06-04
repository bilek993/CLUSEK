#include "EntityEditorWindow.h"
#include "../../ECS/Components/TransformComponent.h"

void EntityEditorWindow::Draw()
{
	ImGui::Begin("Entity Editor", &IsEnabled);

	if (*EntitySelected)
	{
		DrawComponentDetails<TransformComponent>(SelectedEntity, "Transform Component");
	}

	ImGui::End();
}
