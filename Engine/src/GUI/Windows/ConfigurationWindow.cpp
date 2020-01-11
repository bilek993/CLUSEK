#include "ConfigurationWindow.h"
#include <imgui.h>

void ConfigurationWindow::Draw()
{
	ImGui::Begin("Configuration", &IsEnabled);

	DrawSaveButton();
	DrawInfoRestart();

	ImGui::End();
}

void ConfigurationWindow::DrawSaveButton()
{
	if (ImGui::Button("Save"))
	{
		RestartRequired = true;
	}
}

void ConfigurationWindow::DrawInfoRestart() const
{
	if (RestartRequired)
	{
		ImGui::Separator();
		ImGui::Text("To finalize changes, please restart the game engine.");
	}
}
