#pragma once
#include <DirectXMath.h>
#include "../ECS/Components/CameraComponent.h"
#include "../ECS/Components/TransformComponent.h"

class ModelViewLogic final
{
public:
	static void UpdateViewMatrix(CameraComponent &cameraComponent, TransformComponent& transformComponent);
	static void UpdateModelMatrix(DirectX::XMMATRIX& worldMatrix, TransformComponent& transformComponent);
};
