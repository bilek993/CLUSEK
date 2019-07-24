#include "LightingWindow.h"
#include <imgui.h>

void LightingWindow::Draw(LightSettings& lightSettings)
{
	ImGui::Begin("Lighting settings");
	ImGui::Text("Ambient light settings:");
	ImGui::ColorPicker3("Color", 
						&lightSettings.AmbientLightColor.x,
					ImGuiColorEditFlags_NoAlpha | 
						ImGuiColorEditFlags_PickerHueBar | 
						ImGuiColorEditFlags_NoInputs);
	ImGui::DragFloat("Strength", &lightSettings.AmbientLightStrength, 0.001f, 0.0f, 1.0f);
	ImGui::End();
}
