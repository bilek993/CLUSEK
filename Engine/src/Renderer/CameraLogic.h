#pragma once
#include <DirectXMath.h>
#include "../ECS/Components/CameraComponent.h"
#include "../ECS/Components/TransformComponent.h"

class CameraLogic final
{
public:
	static void SetPosition(const DirectX::XMVECTOR &pos, CameraComponent &cameraComponent, TransformComponent &transformComponent);
	static void SetPosition(float x, float y, float z, CameraComponent &cameraComponent, TransformComponent &transformComponent);
	static void SetRotation(const DirectX::XMVECTOR &rot, CameraComponent &cameraComponent, TransformComponent &transformComponent);
	static void SetRotation(float x, float y, float z, CameraComponent &cameraComponent, TransformComponent &transformComponent);

	static void AdjustPosition(const DirectX::XMVECTOR &pos, CameraComponent &cameraComponent, TransformComponent &transformComponent);
	static void AdjustPosition(float x, float y, float z, CameraComponent &cameraComponent, TransformComponent &transformComponent);
	static void AdjustRotation(const DirectX::XMVECTOR &rot, CameraComponent &cameraComponent, TransformComponent &transformComponent);
	static void AdjustRotation(float x, float y, float z, CameraComponent &cameraComponent, TransformComponent &transformComponent);

	static void LookAt(DirectX::XMFLOAT3 targetPosition, CameraComponent &cameraComponent, TransformComponent &transformComponent);

	static void UpdateViewMatrix(CameraComponent &cameraComponent, TransformComponent& transformComponent);
};
