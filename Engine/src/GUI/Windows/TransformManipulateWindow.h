#pragma once
#include "BaseWindow.h"

class TransformManipulateWindow final : public BaseWindow
{
protected:
	void Draw() override;
private:
	void DrawCombo();
	void DrawDetails();

	template <class T>
	void ListComponent(int& counter, std::string& list);

	int SelectedId = -1;
};

template <class T>
void TransformManipulateWindow::ListComponent(int& counter, std::string& list)
{
	Registry->view<T>().each([this, &counter, &list](T& component)
	{
		counter++;
		list += "Transformable entity " + std::to_string(counter) + '\0';
	});
}
