#include "BaseComponentEditor.h"
#include <imgui.h>

void BaseComponentEditor::Update(const float deltaTime, ConfigData* configData, DynamicRenderSettings* renderSettings,
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

void BaseComponentEditor::Draw()
{
	ImGui::Text("No configurable parameters for this component.");
}
