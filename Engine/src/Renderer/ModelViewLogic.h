#pragma once
#include <DirectXMath.h>
#include "../ECS/Components/CameraComponent.h"
#include "../ECS/Components/TransformComponent.h"
#include "../ECS/Components/RenderComponent.h"

class ModelViewLogic final
{
public:
	static void UpdateViewMatrix(CameraComponent &cameraComponent, TransformComponent& transformComponent);
	static void UpdateWorldMatrix(RenderComponent& renderComponent, TransformComponent& transformComponent);
};
