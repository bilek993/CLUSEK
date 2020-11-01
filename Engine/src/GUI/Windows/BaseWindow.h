#pragma once
#include "../../Renderer/DynamicRenderSettings.h"
#include "../../Utils/SystemHolder.h"

struct PostProcessingSettings;

class BaseWindow
{
public:
	virtual ~BaseWindow() = default;
	void Update(float deltaTime, ConfigData *configData, DynamicRenderSettings *renderSettings, 
		std::vector<SystemHolder> *systems, int renderSystemId, int physicsSystemId, IOData *ioData, 
		PostProcessingSettings *postProcessingSettings,	entt::registry *registry, entt::entity *selectedEntity, 
		bool *entitySelected, bool *lockSelecting);

	bool GetIsEnabled() const;
	void SetIsEnabled(bool value);

protected:
	virtual void Draw() = 0;

	bool IsEnabled = false;

	float DeltaTime = 0;
	ConfigData *Config = nullptr;
	DynamicRenderSettings *RenderSettings = nullptr;
	std::vector<SystemHolder> *Systems = nullptr;
	int RenderSystemId = -1;
	int PhysicsSystemId = -1;
	IOData *IoData = nullptr;
	PostProcessingSettings *CurrentPostProcessingSettings = nullptr;
	entt::registry *Registry = nullptr;
	entt::entity *SelectedEntity = nullptr;
	bool *EntitySelected = nullptr;
	bool *LockSelecting = nullptr;
};
