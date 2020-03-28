#pragma once
#include "BaseWindow.h"
#include <imgui.h>
#include <ImGuizmo.h>

class TransformManipulateWindow final : public BaseWindow
{
protected:
	void Draw() override;
private:
	void DrawCombo();
	void DrawDetails();

	template <class T>
	void ListComponent(int& counter, std::string& list);

	ImGuizmo::OPERATION CurrentGizmoOperation = ImGuizmo::TRANSLATE;

	int SelectedId = -1;
	DirectX::XMMATRIX * CurrentWorldMatrix = nullptr;
};

template <class T>
void TransformManipulateWindow::ListComponent(int& counter, std::string& list)
{
	Registry->view<T>().each([this, &counter, &list](T& component)
	{
		list += "Transformable entity " + std::to_string(counter) + '\0';

		if (SelectedId == counter++)
		{
			CurrentWorldMatrix = &component.WorldMatrix;
		}
	});
}
