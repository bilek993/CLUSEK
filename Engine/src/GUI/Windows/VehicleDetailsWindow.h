#pragma once
#include "BaseWindow.h"
#include "../../ECS/Components/VehicleComponent.h"

class VehicleDetailsWindow final : public BaseWindow
{
protected:
	void Draw() override;
private:
	void DrawCombo(entt::basic_view<unsigned int, VehicleComponent> &view);
	void DrawDetails(entt::basic_view<unsigned int, VehicleComponent> &view);

	int SelectedId = -1;
};
