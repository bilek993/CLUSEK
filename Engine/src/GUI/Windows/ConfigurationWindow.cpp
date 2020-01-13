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

void ConfigurationWindow::DrawTextInput()
{
	const auto callback = [](ImGuiInputTextCallbackData *data) 
	{
		const auto textPointer = static_cast<std::string*>(data->UserData);
		textPointer->resize(data->BufTextLen);
		data->Buf = const_cast<char*>(textPointer->c_str());
		return 0;
	};

	ImGui::InputTextMultiline(	"##ConfigurationInput", 
								const_cast<char*>(ConfigurationText.c_str()), 
								ConfigurationText.capacity() + 1,
								ImVec2(0, 0),
								ImGuiInputTextFlags_CallbackResize,
								callback,
								&ConfigurationText);
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
