#include "BaseWindow.h"

void BaseWindow::Update(const float deltaTime, ConfigData *configData, DynamicRenderSettings *renderSettings, 
	std::vector<SystemHolder> *systems, IOData *ioData, PostProcessingSettings *postProcessingSettings)
{
	if (!IsEnabled)
		return;

	Config = configData;
	DeltaTime = deltaTime;
	RenderSettings = renderSettings;
	Systems = systems;
	IoData = ioData;
	CurrentPostProcessingSettings = postProcessingSettings;

	Draw();
}

bool& BaseWindow::GetIsEnabled()
{
	return IsEnabled;
}
