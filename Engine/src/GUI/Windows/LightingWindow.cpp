#include "LightingWindow.h"
#include <imgui.h>

void LightingWindow::Draw(LightSettings& lightSettings)
{
	ImGui::Begin("Lighting settings", &IsEnabled);
	ImGui::Text("Ambient light settings:");
	ImGui::ColorPicker3("Color##Ambient", 
						&lightSettings.AmbientLightColor.x,
					ImGuiColorEditFlags_NoAlpha | 
						ImGuiColorEditFlags_PickerHueBar | 
						ImGuiColorEditFlags_NoInputs);
	ImGui::DragFloat("Strength##Ambient", &lightSettings.AmbientLightStrength, 0.001f, 0.0f, 1.0f);
	ImGui::Separator();
	ImGui::Text("Directional light settings:");
	ImGui::ColorPicker3("Color##Directional",
						&lightSettings.DirectionalLightColor.x,
						ImGuiColorEditFlags_NoAlpha |
						ImGuiColorEditFlags_PickerHueBar |
						ImGuiColorEditFlags_NoInputs);
	ImGui::DragFloat("Strength##Directional", &lightSettings.DirectionalLightStrength, 0.001f, 0.0f, 1.0f);
	ImGui::DragFloat3("Direction##Directional", &lightSettings.DirectionalLightDirection.x, 0.001f, -1.0f, 1.0f);
	ImGui::End();
}
