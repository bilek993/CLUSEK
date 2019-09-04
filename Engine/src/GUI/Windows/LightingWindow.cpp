#include "LightingWindow.h"
#include <imgui.h>

void LightingWindow::Draw()
{
	ImGui::Begin("Lighting settings", &IsEnabled);
	ImGui::Text("Ambient light settings:");
	ImGui::ColorPicker3("Color##Ambient",
		&RenderSettings->AmbientLightColor.x,
		ImGuiColorEditFlags_NoAlpha |
		ImGuiColorEditFlags_PickerHueBar |
		ImGuiColorEditFlags_NoInputs);
	ImGui::DragFloat("Strength##Ambient", &RenderSettings->AmbientLightStrength, 0.005f, 0.0f, 100.0f);
	ImGui::Separator();
	ImGui::Text("Directional light settings:");
	ImGui::ColorPicker3("Color##Directional",
		&RenderSettings->DirectionalLightColor.x,
		ImGuiColorEditFlags_NoAlpha |
		ImGuiColorEditFlags_PickerHueBar |
		ImGuiColorEditFlags_NoInputs);
	ImGui::DragFloat("Strength##Directional", &RenderSettings->DirectionalLightStrength, 0.005f, 0.0f, 100.0f);
	ImGui::DragFloat3("Direction##Directional", &RenderSettings->DirectionalLightDirection.x, 0.001f, -1.0f, 1.0f);
	ImGui::End();
}
