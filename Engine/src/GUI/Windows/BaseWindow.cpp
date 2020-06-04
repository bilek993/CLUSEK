#include "BaseWindow.h"

void BaseWindow::Update(const float deltaTime, ConfigData* configData, DynamicRenderSettings* renderSettings,
	std::vector<SystemHolder>* systems, const int renderSystemId, IOData* ioData,
	PostProcessingSettings* postProcessingSettings, entt::registry *registry, entt::entity *selectedEntity,
	bool *entitySelected)
{
	if (!IsEnabled)
		return;

	Config = configData;
	DeltaTime = deltaTime;
	RenderSettings = renderSettings;
	Systems = systems;
	RenderSystemId = renderSystemId;
	IoData = ioData;
	CurrentPostProcessingSettings = postProcessingSettings;
	Registry = registry;
	SelectedEntity = selectedEntity;
	EntitySelected = entitySelected;

	Draw();
}

bool BaseWindow::GetIsEnabled() const
{
	return IsEnabled;
}

void BaseWindow::SetIsEnabled(const bool value)
{
	IsEnabled = value;
}
