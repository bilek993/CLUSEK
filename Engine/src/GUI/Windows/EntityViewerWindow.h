#pragma once
#include "BaseWindow.h"
#include <imgui.h>

class EntityViewerWindow final : public BaseWindow
{
protected:
	void Draw() override;
private:
	void DrawList();

	template <class T, class E>
	void DrawComponent(E& entity, const std::string& name);
};

template <class T, class E>
void EntityViewerWindow::DrawComponent(E& entity, const std::string& name)
{
	if (Registry->has<T>(entity))
		ImGui::Text(("            " + name).c_str());
}
