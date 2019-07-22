#include "SystemsManagerWindow.h"
#include <imgui.h>

void SystemsManagerWindow::Draw(std::vector<SystemHolder>& systems)
{
	ImGui::Begin("Systems manager", &IsEnabled);
	for (auto& system : systems)
	{
		ImGui::Checkbox(system.Name.c_str(), &system.Enabled);
	}
	ImGui::End();
}
