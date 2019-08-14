#include "GamePadWindow.h"
#include <imgui.h>

void GamePadInputWindow::Draw()
{
	ImGui::Begin("GamePad input", &IsEnabled);

	ImGui::Text(IoData->GamePadState.IsConnected() ? "GamePad connected" : "GamePad is not connected!");

	ImGui::Separator();

	ImGui::Text("Back button: %s", IoData->GamePadState.IsBackPressed() ? "X" : " ");
	ImGui::Text("Start button: %s", IoData->GamePadState.IsStartPressed() ? "X" : " ");

	ImGui::Separator();

	ImGui::Text("A button: %s", IoData->GamePadState.IsAPressed() ? "X" : " ");
	ImGui::Text("X button: %s", IoData->GamePadState.IsXPressed() ? "X" : " ");
	ImGui::Text("Y button: %s", IoData->GamePadState.IsYPressed() ? "X" : " ");
	ImGui::Text("B button: %s", IoData->GamePadState.IsBPressed() ? "X" : " ");

	ImGui::Separator();

	ImGui::Text("LB button: %s", IoData->GamePadState.IsLeftShoulderPressed() ? "X" : " ");
	ImGui::Text("RB button: %s", IoData->GamePadState.IsRightShoulderPressed() ? "X" : " ");

	ImGui::Separator();

	ImGui::Text("Dpad: %s %s %s %s", IoData->GamePadState.dpad.left ? "<" : " ", IoData->GamePadState.dpad.right ? ">" : " ", 
		IoData->GamePadState.dpad.up ? "/\\" : " ", IoData->GamePadState.dpad.down ? "\\/" : " ");

	ImGui::Separator();

	ImGui::Text("Left stick%s: %.2f %.2f", IoData->GamePadState.IsLeftStickPressed() ? "(pressed)" : "", IoData->GamePadState.thumbSticks.leftX, IoData->GamePadState.thumbSticks.leftY);
	ImGui::Text("Right stick%s: %.2f %.2f", IoData->GamePadState.IsRightStickPressed() ? "(pressed)" : "", IoData->GamePadState.thumbSticks.rightX, IoData->GamePadState.thumbSticks.rightY);

	ImGui::Separator();

	ImGui::Text("Left trigger: %.2f", IoData->GamePadState.triggers.left);
	ImGui::Text("Right trigger: %.2f", IoData->GamePadState.triggers.right);

	ImGui::End();
}
