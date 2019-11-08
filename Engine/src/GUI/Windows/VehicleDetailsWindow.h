#pragma once
#include "BaseWindow.h"
#include "../../ECS/Components/VehicleComponent.h"

class VehicleDetailsWindow final : public BaseWindow
{
protected:
	void Draw() override;
private:
	void DrawCombo();
	void DrawDetails() const;

	int SelectedId = -1;
	VehicleComponent* CurrentVehicleComponent = nullptr;
};
