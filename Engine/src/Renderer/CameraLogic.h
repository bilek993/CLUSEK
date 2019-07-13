#pragma once
#include <DirectXMath.h>
#include "../ECS/Components/CameraComponent.h"
#include "../ECS/Components/TransformComponent.h"

class CameraLogic final
{
public:
	static void UpdateViewMatrix(CameraComponent &cameraComponent, TransformComponent& transformComponent);
};
