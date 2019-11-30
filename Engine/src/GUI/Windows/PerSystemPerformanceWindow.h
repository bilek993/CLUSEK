#pragma once
#include "BaseWindow.h"

class PerSystemPerformanceWindow final : public BaseWindow
{
protected:
	void Draw() override;

private:
	void CollectData();
	void DrawSystemNamesWithIds();
	void DrawHistogram();

	std::vector<float> SystemsPerformance;
	std::vector<std::string> SystemNames;
};
