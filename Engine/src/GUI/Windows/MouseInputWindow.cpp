#include "MouseInputWindow.h"
#include <imgui.h>

void MouseInputWindow::Draw()
{
	ImGui::Begin("Mouse input", &IsEnabled);

	ImGui::Text("Left mouse button: %s", IoData->MouseState.leftButton ? "X" : " ");
	ImGui::Text("Middle mouse button: %s", IoData->MouseState.middleButton ? "X" : " ");
	ImGui::Text("Right mouse button: %s", IoData->MouseState.rightButton ? "X" : " ");

	ImGui::Separator();

	ImGui::Text("Mouse x: %d", IoData->MouseState.x);
	ImGui::Text("Mouse y: %d", IoData->MouseState.y);

	ImGui::Separator();

	ImGui::Text("Scroll wheel position: %d", IoData->MouseState.scrollWheelValue);

	ImGui::End();
}
