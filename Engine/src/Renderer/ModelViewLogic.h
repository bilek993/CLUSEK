#pragma once
#include <DirectXMath.h>
#include "../ECS/Components/CameraComponent.h"
#include "../ECS/Components/TransformComponent.h"
#include "../ECS/Components/ModelRenderComponent.h"

class ModelViewLogic final
{
public:
	static void UpdateViewMatrix(CameraComponent &cameraComponent, TransformComponent& transformComponent);
	static void UpdateModelMatrix(ModelRenderComponent& modelRenderComponent, TransformComponent& transformComponent);
};
