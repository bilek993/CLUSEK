#include "DebugUserInterface.h"
#include <imgui.h>

void DebugUserInterface::Update(const IOData& ioData, std::vector<SystemHolder>& systems)
{
	HandleKeyboardEvents(ioData);
	if (!IsEnabled)
		return;

	DrawSystemBrowser(systems);
}

void DebugUserInterface::HandleKeyboardEvents(const IOData& ioData)
{
	if (ioData.KeyboardTracker.released.OemTilde)
		IsEnabled = !IsEnabled;
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
