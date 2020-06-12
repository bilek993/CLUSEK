#pragma once
#include "BaseComponentEditor.h"
#include "../../ECS/Components/VehicleComponent.h"

class VehicleComponentEditor final : public BaseComponentEditor<VehicleComponent>
{
protected:
	void Draw() override;

private:
	void DrawVehicleStats(VehicleComponent* component);
	void DrawWheelsStats(VehicleComponent* component) const;

	void DrawEditableParameters(VehicleComponent* component) const;

	void RecalculateGraph(float rotationSpeed);
	void AddToGraph(float rotationSpeed);

	float TimeCounter = 0.0f;
	float EngineRotationHistory[100] = {};
};
