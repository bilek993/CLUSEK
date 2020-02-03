#pragma once
#include "BaseSystem.h"
#include "../Components/CameraComponent.h"
#include "../Components/TransformComponent.h"

class CameraSystem final : public BaseSystem
{
public:
	void Start() override;
	void Update(float deltaTime) override;
private:
	void HandleMovement(float deltaTime, CameraComponent& cameraComponent, TransformComponent& transformComponent) const;
	void CalculateFrustumPlanes(CameraComponent& cameraComponent) const;

	void GamepadMovement(float deltaTime, CameraComponent& cameraComponent, TransformComponent& transformComponent) const;
	void KeyboardMovement(float deltaTime, CameraComponent& cameraComponent, TransformComponent& transformComponent) const;
	void MouseMovement(float deltaTime, CameraComponent& cameraComponent, TransformComponent& transformComponent) const;
};
