#pragma once
#include "../ECS/Components/TransformComponent.h"
#include "../ECS/Components/CameraComponent.h"
#include <entity/registry.hpp>

class CameraLocator final
{
public:
	static CameraComponent& GetMainCamera(entt::registry *registry);
	static TransformComponent& GetMainCameraTransform(entt::registry *registry);
};
