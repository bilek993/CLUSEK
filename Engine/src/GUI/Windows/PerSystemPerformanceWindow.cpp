#include "PerSystemPerformanceWindow.h"
#include <imgui.h>

void PerSystemPerformanceWindow::Draw()
{
	ImGui::Begin("Per System Performance Window", &IsEnabled);

	for (auto& system : *Systems)
	{
		ImGui::Text("- %s %f", system.Name.c_str(), system.System->GetDeltaTimeAndRestart());
	}

	ImGui::End();
}
