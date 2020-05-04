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

	float GetCameraSpeed(float deltaTime) const;
	bool GetTargetMode(const CameraComponent& cameraComponent) const;
	DirectX::XMVECTOR GetTranslation(float cameraSpeed, const CameraComponent& cameraComponent) const;
	std::pair<float, float> GetRotation(float deltaTime, const CameraComponent& cameraComponent, const TransformComponent& transformComponent) const;

	void UpdateViewMatrix(CameraComponent &cameraComponent, TransformComponent& transformComponent) const;
};
