#include "ConfigurationWindow.h"
#include <imgui.h>

void ConfigurationWindow::Draw()
{
	ImGui::Begin("Configuration", &IsEnabled);


	ImGui::End();
}
