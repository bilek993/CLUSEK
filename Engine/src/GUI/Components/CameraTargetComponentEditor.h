#pragma once
#include "BaseComponentEditor.h"
#include "../../ECS/Components/CameraTargetComponent.h"

class CameraTargetComponentEditor final : public BaseComponentEditor<CameraTargetComponent>
{
protected:
	void Draw() override;

	void HandleIncorrectValues(CameraTargetComponent* component) const;
};
