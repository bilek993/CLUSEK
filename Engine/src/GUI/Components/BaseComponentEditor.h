#pragma once
#include "../../Renderer/DynamicRenderSettings.h"
#include "../../Utils/SystemHolder.h"

class BaseComponentEditor
{
public:
	void Update(float deltaTime, ConfigData *configData, DynamicRenderSettings *renderSettings,
		std::vector<SystemHolder> *systems, int renderSystemId, IOData *ioData, PostProcessingSettings *postProcessingSettings,
		entt::registry *registry, entt::entity *selectedEntity, bool *entitySelected);

protected:
	virtual void Draw() = 0;

	float DeltaTime = 0;
	ConfigData *Config = nullptr;
	DynamicRenderSettings *RenderSettings = nullptr;
	std::vector<SystemHolder> *Systems = nullptr;
	int RenderSystemId = -1;
	IOData *IoData = nullptr;
	PostProcessingSettings *CurrentPostProcessingSettings = nullptr;
	entt::registry *Registry = nullptr;
	entt::entity *SelectedEntity = nullptr;
	bool *EntitySelected = nullptr;
};