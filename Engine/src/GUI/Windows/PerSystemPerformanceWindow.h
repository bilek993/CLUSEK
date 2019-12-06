#pragma once
#include "BaseWindow.h"
#include <map>

class PerSystemPerformanceWindow final : public BaseWindow
{
protected:
	void Draw() override;

private:
	void CollectData();
	void DrawDeltaTime() const;
	void DrawSystemNamesWithIds();
	void DrawHistogram();

	std::vector<float> SystemsPerformance;
	std::vector<float> SystemsWorstPerformance;
	std::vector<std::string> SystemNames;

	std::map<std::string, float> SystemsHighestValue;
};
