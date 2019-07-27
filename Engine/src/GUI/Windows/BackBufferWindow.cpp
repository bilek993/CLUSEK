#include "BackBufferWindow.h"
#include <imgui.h>

void BackBufferWindow::Draw(DynamicRenderSettings& renderSettings)
{
	ImGui::Begin("Back Buffer settings", &IsEnabled);
	ImGui::ColorPicker3("Clear color",
		renderSettings.ClearColor,
		ImGuiColorEditFlags_NoAlpha |
		ImGuiColorEditFlags_PickerHueBar |
		ImGuiColorEditFlags_NoInputs);
	ImGui::End();
}
