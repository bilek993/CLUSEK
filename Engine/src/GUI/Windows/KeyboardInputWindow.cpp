#include "KeyboardInputWindow.h"
#include <imgui.h>

void KeyboardInputWindow::Draw()
{
	ImGui::Begin("Keyboard input", &IsEnabled);
	ImGui::End();
}
