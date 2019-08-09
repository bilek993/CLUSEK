#include "KeyboardInputWindow.h"
#include <imgui.h>

void KeyboardInputWindow::Draw()
{
	auto& io = ImGui::GetIO();

	ImGui::Begin("Keyboard input", &IsEnabled);

	ImGui::Text("Keys down:"); 
	for (auto i = 0; i < IM_ARRAYSIZE(io.KeysDown); i++)
	{
		if (io.KeysDownDuration[i] >= 0.0f)
		{
			ImGui::SameLine(); ImGui::Text("%d (0x%X) (%.02f sec)", i, i, io.KeysDownDuration[i]);
		}
	}
	
	ImGui::Text("Keys pressed:");
	for (auto i = 0; i < IM_ARRAYSIZE(io.KeysDown); i++)
	{
		if (ImGui::IsKeyPressed(i))
		{
			ImGui::SameLine(); ImGui::Text("%d (0x%X)", i, i);
		}
	}
	
	ImGui::Text("Keys release:");
	for (auto i = 0; i < IM_ARRAYSIZE(io.KeysDown); i++) {
		if (ImGui::IsKeyReleased(i))
		{
			ImGui::SameLine(); ImGui::Text("%d (0x%X)", i, i);
		}
	}
	
	ImGui::End();
}
