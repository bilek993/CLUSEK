#pragma once
#include "../../Renderer/DynamicRenderSettings.h"
#include "../../Utils/SystemHolder.h"
#include <imgui.h>

template <class C>
class BaseComponentEditor
{
public:
	virtual ~BaseComponentEditor() = default;
	void Update(float deltaTime, ConfigData *configData, DynamicRenderSettings *renderSettings,
		std::vector<SystemHolder> *systems, int renderSystemId, IOData *ioData, PostProcessingSettings *postProcessingSettings,
		entt::registry *registry, entt::entity *selectedEntity, bool *entitySelected);

protected:
	// It is recommended to override this function
	virtual void Draw();

	C* GetPointerToThisComponent();

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

template <class C>
void BaseComponentEditor<C>::Update(const float deltaTime, ConfigData* configData, DynamicRenderSettings* renderSettings,
	std::vector<SystemHolder>* systems, const int renderSystemId, IOData* ioData,
	PostProcessingSettings* postProcessingSettings, entt::registry* registry, entt::entity* selectedEntity,
	bool* entitySelected)
{
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

template <class C>
void BaseComponentEditor<C>::Draw()
{
	ImGui::Text("No configurable parameters for this component.");
}

template <class C>
C* BaseComponentEditor<C>::GetPointerToThisComponent()
{
	return &Registry->get<C>(*SelectedEntity);
}
