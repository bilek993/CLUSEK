#include "BackBufferWindow.h"
#include <imgui.h>

void BackBufferWindow::Draw()
{
	ImGui::Begin("Back Buffer settings", &IsEnabled);
	ImGui::ColorPicker3("Clear color",
		RenderSettings->ClearColor,
		ImGuiColorEditFlags_NoAlpha |
		ImGuiColorEditFlags_PickerHueBar |
		ImGuiColorEditFlags_NoInputs);
	ImGui::End();
}
