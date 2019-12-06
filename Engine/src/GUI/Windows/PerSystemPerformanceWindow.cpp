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
	SystemsWorstPerformance.clear();

	for (auto& system : *Systems)
	{
		if (!system.Enabled)
			continue;

		const auto systemDelta = system.System->GetDeltaTime();

		if (SystemsHighestValue.find(system.Name) == SystemsHighestValue.end())
		{
			SystemsHighestValue.emplace(system.Name, systemDelta);

			SystemsWorstPerformance.emplace_back(systemDelta);
		}
		else
		{
			auto& currentHighestValue = SystemsHighestValue[system.Name];

			if (currentHighestValue < systemDelta)
				currentHighestValue = systemDelta;

			SystemsWorstPerformance.emplace_back(currentHighestValue);
		}

		SystemsPerformance.emplace_back(systemDelta);
		SystemNames.emplace_back(system.Name);
	}
}

void PerSystemPerformanceWindow::DrawDeltaTime() const
{
	ImGui::Text("Delta time: %.2f ms", DeltaTime);
}

void PerSystemPerformanceWindow::DrawSystemNamesWithIds()
{
	for (auto i = 0; i < SystemsPerformance.size(); i++)
		ImGui::BulletText("[%d] %s - %.2f ms (max: %.2f ms)", i, SystemNames[i].c_str(), SystemsPerformance[i], SystemsWorstPerformance[i]);
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
