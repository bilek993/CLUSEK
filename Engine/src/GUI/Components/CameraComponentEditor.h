#pragma once
#include "BaseComponentEditor.h"
#include "../../ECS/Components/CameraComponent.h"

class CameraComponentEditor final : public BaseComponentEditor<CameraComponent>
{
protected:
	void Draw() override;
};
