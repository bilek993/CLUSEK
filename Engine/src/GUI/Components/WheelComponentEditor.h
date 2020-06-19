#pragma once
#include "BaseComponentEditor.h"
#include "../../ECS/Components/WheelComponent.h"
#include "PxPhysicsAPI.h"

class WheelComponentEditor final : public BaseComponentEditor<WheelComponent>
{
protected:
	void Draw() override;

private:
	void DrawBasicConfig(WheelComponent* wheelComponent) const;
	void DrawWheelData(physx::PxVehicleWheelsSimData& wheelsSimData, int wheelId) const;
	void DrawSuspensionData(physx::PxVehicleWheelsSimData& wheelsSimData, int wheelId) const;
};
