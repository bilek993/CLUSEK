#include "FogWindow.h"
#include <imgui.h>

void FogWindow::Draw()
{
	ImGui::Begin("Fog settings", &IsEnabled);
	ImGui::End();
}
