#pragma once
#include <DirectXMath.h>
#include "../ECS/Components/CameraComponent.h"

class CameraLogic final
{
public:
	static void SetPosition(const DirectX::XMVECTOR &pos, CameraComponent &cameraComponent);
	static void SetPosition(float x, float y, float z, CameraComponent &cameraComponent);
	static void SetRotation(const DirectX::XMVECTOR &rot, CameraComponent &cameraComponent);
	static void SetRotation(float x, float y, float z, CameraComponent &cameraComponent);

	static void AdjustPosition(const DirectX::XMVECTOR &pos, CameraComponent &cameraComponent);
	static void AdjustPosition(float x, float y, float z, CameraComponent &cameraComponent);
	static void AdjustRotation(const DirectX::XMVECTOR &rot, CameraComponent &cameraComponent);
	static void AdjustRotation(float x, float y, float z, CameraComponent &cameraComponent);

	static void LookAt(DirectX::XMFLOAT3 targetPosition, CameraComponent &cameraComponent);

	static void UpdateViewMatrix(CameraComponent &cameraComponent);
};
