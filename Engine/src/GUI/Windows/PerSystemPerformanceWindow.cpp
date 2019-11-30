#include "PerSystemPerformanceWindow.h"
#include <imgui.h>

void PerSystemPerformanceWindow::Draw()
{
	CollectData();

	ImGui::Begin("Per System Performance Window", &IsEnabled);

	DrawDeltaTime();
	ImGui::Separator();
	DrawSystemNamesWithIds();
	ImGui::Separator();
	DrawHistogram();

	ImGui::End();
}

void PerSystemPerformanceWindow::CollectData()
{
	SystemsPerformance.clear();
	SystemNames.clear();

	for (auto& system : *Systems)
	{
		if (!system.Enabled)
			continue;

		SystemsPerformance.emplace_back(system.System->GetDeltaTime());
		SystemNames.emplace_back(system.Name);
	}
}

void PerSystemPerformanceWindow::DrawDeltaTime() const
{
	ImGui::Text("Delta time: %f", DeltaTime);
}

void PerSystemPerformanceWindow::DrawSystemNamesWithIds()
{
	for (auto i = 0; i < SystemsPerformance.size(); i++)
		ImGui::BulletText("[%d] %s - %f ms", i, SystemNames[i].c_str(), SystemsPerformance[i]);
}

void PerSystemPerformanceWindow::DrawHistogram()
{
	ImGui::PlotHistogram(	"Current Systems Performance", 
							SystemsPerformance.data(), 
							SystemsPerformance.size(), 
							0, 
							nullptr, 
							0.0f, 
							DeltaTime, 
							ImVec2(0, 100));
}
