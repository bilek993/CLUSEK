#pragma once
#include "BaseSystem.h"
#include "../Components/CameraComponent.h"

class CameraSystem final : public BaseSystem
{
public:

	void Start(entt::registry& registry, const ConfigData& configData, Renderer &renderer) override;
	void Update(float deltaTime, entt::registry &registry, IOData& ioData, IODevices &ioDevices,
		Renderer &renderer, RenderWindow &window, ConfigData &configData) override;
private:
	void SetPosition(const DirectX::XMVECTOR &pos, CameraComponent &cameraComponent);
	void SetPosition(float x, float y, float z, CameraComponent &cameraComponent);
	void SetRotation(const DirectX::XMVECTOR &rot, CameraComponent &cameraComponent);
	void SetRotation(float x, float y, float z, CameraComponent &cameraComponent);

	void AdjustPosition(const DirectX::XMVECTOR &pos, CameraComponent &cameraComponent);
	void AdjustPosition(float x, float y, float z, CameraComponent &cameraComponent);
	void AdjustRotation(const DirectX::XMVECTOR &rot, CameraComponent &cameraComponent);
	void AdjustRotation(float x, float y, float z, CameraComponent &cameraComponent);

	void LookAt(DirectX::XMFLOAT3 targetPosition, CameraComponent &cameraComponent);

	void UpdateViewMatrix(CameraComponent &cameraComponent);
};
