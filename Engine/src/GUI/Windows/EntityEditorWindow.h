#pragma once
#include "BaseWindow.h"
#include <imgui.h>
#include "../Components/TransformComponentEditor.h"

#define REGISTER_COMPONENT_EDITOR(COMPONENT_CLASS, COMPONENT_NAME) DrawComponentDetails<COMPONENT_CLASS>(SelectedEntity, COMPONENT_CLASS##EditorInstance, COMPONENT_NAME)

class EntityEditorWindow final : public BaseWindow
{
protected:
	void Draw() override;

private:
	void DrawComponentsForEntity();
	void DrawNoElements() const;

	template <class T>
	void DrawComponentDetails(const entt::entity* entity, BaseComponentEditor& editorInstance, const std::string& name) const;

	TransformComponentEditor TransformComponentEditorInstance;
};

template <class T>
void EntityEditorWindow::DrawComponentDetails(const entt::entity* entity, BaseComponentEditor& editorInstance, const std::string& name) const
{
	if (!Registry->has<T>(*entity))
		return;

	if (ImGui::CollapsingHeader(name.c_str()))
	{
		editorInstance.Update(	DeltaTime, 
								Config, 
								RenderSettings, 
								Systems, 
								RenderSystemId,
								IoData, 
								CurrentPostProcessingSettings, 
								Registry, 
								SelectedEntity, 
								EntitySelected);
	}
}
