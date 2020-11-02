#include "EntitySelectorLockWindow.h"
#include <imgui.h>

void EntitySelectorLockWindow::Draw()
{
	ImGui::Begin("Entity selector lock", &IsEnabled);
		

	if (*LockSelecting)
	{
		ImGui::Text("Entities selection is locked!");
		ImGui::SameLine();

		if (ImGui::Button("Unlock"))
			*LockSelecting = false;
	}
	else
	{
		if (ImGui::Button("Lock"))
			*LockSelecting = true;
	}
	
	ImGui::End();
}
