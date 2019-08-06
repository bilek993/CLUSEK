#include "RotationSystem.h"
#include "../Components/ModelRenderComponent.h"
#include "../Components/TransformComponent.h"
#include "../../Renderer/TransformLogic.h"
#include "../../Renderer/ModelViewLogic.h"

void RotationSystem::Start()
{
	Logger::Debug("Staring rotation system...");
	Registry->view<TransformComponent, ModelRenderComponent>().each([](TransformComponent &transformComponent, ModelRenderComponent &modelRenderComponent)
	{
		ModelViewLogic::UpdateModelMatrix(modelRenderComponent.WorldMatrix, transformComponent);
	});
}

void RotationSystem::Update(const float deltaTime)
{
	Registry->view<TransformComponent, ModelRenderComponent>().each([&deltaTime](TransformComponent &transformComponent, ModelRenderComponent &modelRenderComponent)
	{
		TransformLogic::AdjustRotation(0.0f, 0.001f * deltaTime, 0.0f, transformComponent);
		ModelViewLogic::UpdateModelMatrix(modelRenderComponent.WorldMatrix, transformComponent);
	});
}
