#include "SystemsManagerWindow.h"
#include <imgui.h>

bool SystemsManagerWindow::IsEnabled = false;

void SystemsManagerWindow::Draw(std::vector<SystemHolder>& systems)
{
	ImGui::Begin("Systems manager", &IsEnabled);
	for (auto& system : systems)
	{
		ImGui::Checkbox(system.Name.c_str(), &system.Enabled);
	}
	ImGui::End();
}
