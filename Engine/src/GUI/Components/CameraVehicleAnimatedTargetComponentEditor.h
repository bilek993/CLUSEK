#pragma once
#include "BaseComponentEditor.h"
#include "../../ECS/Components/CameraVehicleAnimatedTargetComponent.h"

class CameraVehicleAnimatedTargetComponentEditor final : public BaseComponentEditor<CameraVehicleAnimatedTargetComponent>
{
protected:
	void Draw() override;

private:
	void DrawAnimationPositionVectorEditor(CameraVehicleAnimatedTargetComponent* component) const;
};
