#include "GrassWindow.h"

#include <imgui.h>

void GrassWindow::Draw()
{
	ImGui::Begin("Grass settings", &IsEnabled);

	ImGui::Checkbox("Enabled", &RenderSettings->GrassEnabled);
	
	ImGui::End();
}
