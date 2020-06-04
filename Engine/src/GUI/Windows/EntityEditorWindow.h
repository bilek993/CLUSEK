#pragma once
#include "BaseWindow.h"
#include <imgui.h>

class EntityEditorWindow final : public BaseWindow
{
protected:
	void Draw() override;

private:
	template <class T>
	void DrawComponentDetails(entt::entity* entity, const std::string& name) const;
};

template <class T>
void EntityEditorWindow::DrawComponentDetails(entt::entity* entity, const std::string& name) const
{
	if (!Registry->has<T>(*entity))
		return;

	if (ImGui::TreeNode(name.c_str()))
	{
		ImGui::TreePop();
	}
}
