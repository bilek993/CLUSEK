#include "ConfigurationWindow.h"
#include <imgui.h>

void ConfigurationWindow::Draw()
{
	ImGui::Begin("Configuration", &IsEnabled);

	DrawTextInput();
	DrawSaveButton();
	DrawInfoRestart();

	ImGui::End();
}

void ConfigurationWindow::DrawTextInput() const
{
	ImGui::InputTextMultiline(	"##ConfigurationInput", 
								const_cast<char*>(ConfigurationText.c_str()), 
								ConfigurationText.capacity() + 1,
								ImVec2(0, 0));
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
