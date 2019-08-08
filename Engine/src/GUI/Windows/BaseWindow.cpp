#include "BaseWindow.h"

void BaseWindow::Update(const float deltaTime, DynamicRenderSettings *renderSettings, std::vector<SystemHolder> *systems, IOData *ioData)
{
	if (!IsEnabled)
		return;

	DeltaTime = deltaTime;
	RenderSettings = renderSettings;
	Systems = systems;
	IoData = ioData;

	Draw();
}

bool& BaseWindow::GetIsEnabled()
{
	return IsEnabled;
}
