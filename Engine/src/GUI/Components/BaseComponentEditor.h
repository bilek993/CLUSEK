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
		std::vector<SystemHolder> *systems, int renderSystemId, int physicsSystemId, IOData *ioData, 
		PostProcessingSettings *postProcessingSettings,	entt::registry *registry, entt::entity *selectedEntity, 
		bool *entitySelected, bool *lockSelecting, ImDrawList* fullscreenDrawList);

protected:
	// It is recommended to override this function
	virtual void Draw();

	C* GetPointerToThisComponent();

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
	ImDrawList* FullscreenDrawList = nullptr;
};

template <class C>
void BaseComponentEditor<C>::Update(const float deltaTime, ConfigData* configData, DynamicRenderSettings* renderSettings,
	std::vector<SystemHolder>* systems, const int renderSystemId, const int physicsSystemId, IOData* ioData,
	PostProcessingSettings* postProcessingSettings, entt::registry* registry, entt::entity* selectedEntity,
	bool* entitySelected, bool *lockSelecting, ImDrawList* fullscreenDrawList)
{
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
	FullscreenDrawList = fullscreenDrawList;

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
