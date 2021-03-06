#pragma once
#include "BaseSystem.h"
#include "../Components/CameraComponent.h"
#include "../Components/TransformComponent.h"
#include "../Components/CameraTargetComponent.h"
#include "../Components/VehicleComponent.h"
#include "../Components/CameraVehicleAnimatedTargetComponent.h"

class CameraSystem final : public BaseSystem
{
public:
	void Start() override;
	void Rebuild() override;
	void Update(float deltaTime) override;
private:
	void HandleMovement(float deltaTime, CameraComponent& mainCameraCameraComponent, TransformComponent& mainCameraTransformComponent,
		const CameraTargetComponent* cameraTargetComponent, const TransformComponent* cameraTargetTransformComponent,
		CameraVehicleAnimatedTargetComponent* cameraVehicleAnimatedTargetComponent, const VehicleComponent* vehicleComponent) const;

	float GetCameraSpeed(float deltaTime) const;
	bool GetTargetMode(CameraComponent& mainCameraCameraComponent) const;
	DirectX::XMVECTOR GetTranslation(float cameraSpeed, const CameraComponent& mainCameraCameraComponent) const;
	std::pair<float, float> GetRotation(float deltaTime, const CameraComponent& mainCameraCameraComponent, const TransformComponent& mainCameraTransformComponent) const;

	DirectX::XMMATRIX CalculateLerpMatrix(float deltaTime, const CameraTargetComponent* cameraTargetComponent, 
		const TransformComponent* cameraTargetTransformComponent, const TransformComponent& mainCameraTransformComponent,
		CameraVehicleAnimatedTargetComponent* cameraVehicleAnimatedTargetComponent, const VehicleComponent* vehicleComponent) const;
	DirectX::XMMATRIX CalculateVehicleAnimation(float deltaTime, const TransformComponent* cameraTargetTransformComponent,
		CameraVehicleAnimatedTargetComponent* cameraVehicleAnimatedTargetComponent, const VehicleComponent* vehicleComponent) const;

	void UpdateViewMatrix(CameraComponent &mainCameraCameraComponent, TransformComponent& mainCameraTransformComponent, bool targeted) const;
};
