#pragma once
#include "BaseWindow.h"
#include <imgui.h>

class EntityViewerWindow final : public BaseWindow
{
protected:
	void Draw() override;
private:
	void DrawList();

	template <class T>
	void DrawComponent(entt::entity& entity, const std::string& name);
};

template <class T>
void EntityViewerWindow::DrawComponent(entt::entity& entity, const std::string& name)
{
	if (Registry->has<T>(entity))
		ImGui::Text(("            " + name).c_str());
}
