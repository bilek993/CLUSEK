#pragma once
#include "BaseWindow.h"
#include "../../ECS/Components/VehicleComponent.h"

class VehicleDetailsWindow final : public BaseWindow
{
protected:
	void Draw() override;
private:
	void DrawCombo();
	void DrawDetails();
	void RecalculateGraph(float rotationSpeed);
	void AddToGraph(float rotationSpeed);

	int SelectedId = -1;
	VehicleComponent* CurrentVehicleComponent = nullptr;

	float TimeCounter = 0.0f;
	float EngineRotationHistory[50];
};
