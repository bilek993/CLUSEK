#include "CameraSpeedWindow.h"
#include <imgui.h>

void CameraSpeedWindow::Draw()
{
	ImGui::Begin("Camera speed", &IsEnabled);

	ImGui::DragFloat("Movement speed", &Config->MaxCameraSpeed, 0.0001f, 0.0f);

	ImGui::End();
}
