#include "ConfigurationWindow.h"
#include <imgui.h>
#include <fstream>

void ConfigurationWindow::Draw()
{
	ImGui::Begin("Configuration", &IsEnabled);

	if (!ConfigLoaded)
		LoadConfiguration();

	DrawTextInput();
	DrawSaveButton();
	DrawInfoRestart();

	ImGui::End();
}

void ConfigurationWindow::LoadConfiguration()
{
	ConfigLoaded = true;

	std::ifstream cfgFile(ConfigFilePath);
	if (cfgFile.is_open())
	{
		std::string line;

		while (getline(cfgFile, line))
		{
			ConfigurationText += line;
			ConfigurationText += "\n";
		}
	}
	else
	{
		Logger::Error("Cannot find configuration file!");
	}

	cfgFile.close();
}

void ConfigurationWindow::SaveConfiguration() const
{
	std::ofstream cfgFile(ConfigFilePath);
	if (cfgFile.is_open())
		cfgFile << ConfigurationText;
	else
		Logger::Error("Error creating configuration file!");

	cfgFile.close();
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
								ImVec2(-FLT_MIN, 350.0f),
								ImGuiInputTextFlags_CallbackResize,
								callback,
								&ConfigurationText);
}

void ConfigurationWindow::DrawSaveButton()
{
	if (ImGui::Button("Save"))
	{
		RestartRequired = true;
		SaveConfiguration();
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
