#include "BaseWindow.h"

void BaseWindow::Update(const float deltaTime, ConfigData *configData, DynamicRenderSettings *renderSettings, std::vector<SystemHolder> *systems, IOData *ioData)
{
	if (!IsEnabled)
		return;

	Config = configData;
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
