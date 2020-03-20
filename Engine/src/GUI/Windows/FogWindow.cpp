#include "FogWindow.h"
#include <imgui.h>

void FogWindow::Draw()
{
	ImGui::Begin("Fog settings", &IsEnabled);
	ImGui::Text("Fog color:");
	ImGui::ColorPicker3("Color##Fog",
		&RenderSettings->FogColor.x,
		ImGuiColorEditFlags_NoAlpha |
		ImGuiColorEditFlags_PickerHueBar |
		ImGuiColorEditFlags_NoInputs);
	ImGui::DragFloat("Fog density", &RenderSettings->FogDensity, 0.0001f, 0.0f, 0.0f, "%.5f");
	ImGui::DragFloat("Sky constant value", &RenderSettings->SkyConstantValue, 1.0f);
	ImGui::End();
}
