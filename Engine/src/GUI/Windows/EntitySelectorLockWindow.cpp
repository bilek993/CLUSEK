#include "EntitySelectorLockWindow.h"

#include <imgui.h>

void EntitySelectorLockWindow::Draw()
{
	ImGui::Begin("Entity selector lock", &IsEnabled);

	ImGui::End();
}
