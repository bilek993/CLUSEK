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

	for (auto& system : *Systems)
		SystemsPerformance.emplace_back(system.System->GetDeltaTime());
}

void PerSystemPerformanceWindow::DrawHistogram()
{
	ImGui::PlotHistogram("Histogram", SystemsPerformance.data(), SystemsPerformance.size(), 0, nullptr, 0.0f, 10.0f, ImVec2(0, 80));
}
