#include "CameraLocator.h"
#include <entt.hpp>
#include "Logger.h"
#include "../Tags.h"

CameraComponent& CameraLocator::GetMainCamera(entt::registry* registry)
{
	const auto view = registry->view<CameraComponent, entt::tag<Tags::MAIN_CAMERA>>();
	if (view.size() != 1)
	{
		if (view.size() > 1)
			Logger::Error("More than one main render camera found!");
		else
			Logger::Error("Main render camera not found!");
	}

	return view.raw<CameraComponent>()[0];
}

TransformComponent& CameraLocator::GetMainCameraTransform(entt::registry* registry)
{
	const auto view = registry->view<CameraComponent, TransformComponent, entt::tag<Tags::MAIN_CAMERA>>();
	if (view.size() != 1)
	{
		if (view.size() > 1)
			Logger::Error("More than one main render camera found!");
		else
			Logger::Error("Main render camera not found!");
	}

	return view.raw<TransformComponent>()[0];
}
