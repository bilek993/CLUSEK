#include "MouseInputWindow.h"
#include <imgui.h>

void MouseInputWindow::Draw()
{
	ImGui::Begin("Mouse input", &IsEnabled);
	ImGui::End();
}
