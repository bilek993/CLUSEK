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

	inline static const float EPSILON = 0.001f;

	float TimeCounter = 0.0f;
	float EngineRotationHistory[100] = {};

	inline static const char* DifferentialTypes[] = {	"LIMITED_SLIP_4W", 
														"LIMITED_SLIP_FRONT", 
														"LIMITED_SLIP_REAR", 
														"OPEN_4W", 
														"OPEN_FRONT", 
														"OPEN_REAR"};
	inline static const char* ClutchAccuracyModes[] = {	"ESTIMATE",
														"BEST_POSSIBLE" };
};
