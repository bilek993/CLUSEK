#include "LightingWindow.h"
#include <imgui.h>

void LightingWindow::Draw(DynamicRenderSettings& renderSettings)
{
	ImGui::Begin("Lighting settings", &IsEnabled);
	ImGui::Text("Ambient light settings:");
	ImGui::ColorPicker3("Color##Ambient", 
						&renderSettings.AmbientLightColor.x,
					ImGuiColorEditFlags_NoAlpha | 
						ImGuiColorEditFlags_PickerHueBar | 
						ImGuiColorEditFlags_NoInputs);
	ImGui::DragFloat("Strength##Ambient", &renderSettings.AmbientLightStrength, 0.001f, 0.0f, 1.0f);
	ImGui::Separator();
	ImGui::Text("Directional light settings:");
	ImGui::ColorPicker3("Color##Directional",
						&renderSettings.DirectionalLightColor.x,
						ImGuiColorEditFlags_NoAlpha |
						ImGuiColorEditFlags_PickerHueBar |
						ImGuiColorEditFlags_NoInputs);
	ImGui::DragFloat("Strength##Directional", &renderSettings.DirectionalLightStrength, 0.001f, 0.0f, 1.0f);
	ImGui::DragFloat3("Direction##Directional", &renderSettings.DirectionalLightDirection.x, 0.001f, -1.0f, 1.0f);
	ImGui::End();
}
