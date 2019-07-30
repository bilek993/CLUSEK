#include "SystemsManagerWindow.h"
#include <imgui.h>

void SystemsManagerWindow::Draw()
{
	ImGui::Begin("Systems manager", &IsEnabled);
	for (auto& system : *Systems)
	{
		ImGui::Checkbox(system.Name.c_str(), &system.Enabled);
	}
	ImGui::End();
}
