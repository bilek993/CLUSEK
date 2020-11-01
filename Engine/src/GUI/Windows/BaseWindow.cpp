#include "BaseWindow.h"

void BaseWindow::Update(const float deltaTime, ConfigData* configData, DynamicRenderSettings* renderSettings,
	std::vector<SystemHolder>* systems, const int renderSystemId, int physicsSystemId, IOData* ioData,
	PostProcessingSettings* postProcessingSettings, entt::registry *registry, entt::entity *selectedEntity,
	bool *entitySelected, bool *lockSelecting)
{
	if (!IsEnabled)
		return;

	Config = configData;
	DeltaTime = deltaTime;
	RenderSettings = renderSettings;
	Systems = systems;
	RenderSystemId = renderSystemId;
	PhysicsSystemId = physicsSystemId;
	IoData = ioData;
	CurrentPostProcessingSettings = postProcessingSettings;
	Registry = registry;
	SelectedEntity = selectedEntity;
	EntitySelected = entitySelected;
	LockSelecting = lockSelecting;

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
