#include "PerSystemPerformanceWindow.h"
#include <imgui.h>

void PerSystemPerformanceWindow::Draw()
{
	CollectData();

	ImGui::Begin("Per System Performance Window", &IsEnabled);

	DrawHistogram();

	ImGui::End();
}

void PerSystemPerformanceWindow::CollectData()
{
	SystemsPerformance.clear();
	SystemNames.clear();

	for (auto& system : *Systems)
	{
		SystemsPerformance.emplace_back(system.System->GetDeltaTime());
		SystemNames.emplace_back(system.Name);
	}
}

void PerSystemPerformanceWindow::DrawHistogram()
{
	ImGui::PlotHistogram(	"Current System Performance", 
							SystemsPerformance.data(), 
							SystemsPerformance.size(), 
							0, 
							nullptr, 
							0.0f, 
							DeltaTime, 
							ImVec2(0, 100));
}
