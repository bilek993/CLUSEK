#include "DebugUserInterface.h"
#include <imgui.h>

void DebugUserInterface::Update(std::vector<SystemHolder>& systems) const
{
	DrawSystemBrowser(systems);
}

void DebugUserInterface::DrawSystemBrowser(std::vector<SystemHolder>& systems) const
{
	ImGui::Begin("Systems browser");
	for (auto& system : systems)
	{
		ImGui::Checkbox(system.Name.c_str(), &system.Enabled);
	}
	ImGui::End();
}
