#include "SkyboxSystem.h"
#include "../Components/SkyboxComponent.h"
#include "../Components/TransformComponent.h"
#include "../../Renderer/ModelViewLogic.h"
#include "../../Renderer/TransformLogic.h"

void SkyboxSystem::Start()
{
}

void SkyboxSystem::Update(float deltaTime)
{
	auto view = Registry->view<CameraComponent, TransformComponent>();
	if (view.size() != 1)
	{
		if (view.size() > 1)
			Logger::Error("More than one main render camera found!");
		else
			Logger::Error("Main render camera not found!");
	}

	auto &cameraTransformComponent = view.raw<TransformComponent>()[0];

	Registry->view<TransformComponent, SkyboxComponent>().each([&deltaTime, &cameraTransformComponent](TransformComponent &transformComponent, SkyboxComponent &skyboxComponent)
	{
		TransformLogic::SetPosition(cameraTransformComponent.PositionVector, transformComponent);
		ModelViewLogic::UpdateModelMatrix(skyboxComponent.WorldMatrix, transformComponent);
	});
}
