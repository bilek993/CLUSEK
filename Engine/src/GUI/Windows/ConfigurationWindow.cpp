#include "ConfigurationWindow.h"
#include <imgui.h>
#include <fstream>
#include "../../Utils/ImGuiExtensions.h"

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
	ImGui::InputTextMultiline(	"##ConfigurationInput", 
								const_cast<char*>(ConfigurationText.c_str()), 
								ConfigurationText.capacity() + 1,
								ImVec2(-FLT_MIN, 350.0f),
								ImGuiInputTextFlags_CallbackResize,
								ImGuiExtensions::InputTextStringCallback,
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
