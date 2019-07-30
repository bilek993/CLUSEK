#include "BaseWindow.h"

void BaseWindow::Update(const float deltaTime, DynamicRenderSettings *renderSettings, std::vector<SystemHolder> *systems)
{
	if (!IsEnabled)
		return;

	DeltaTime = deltaTime;
	RenderSettings = renderSettings;
	Systems = systems;

	Draw();
}

bool& BaseWindow::GetIsEnabled()
{
	return IsEnabled;
}
