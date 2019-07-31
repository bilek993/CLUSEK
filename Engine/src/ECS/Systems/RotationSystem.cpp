#include "RotationSystem.h"
#include "../Components/RenderComponent.h"
#include "../Components/TransformComponent.h"
#include "../../Renderer/TransformLogic.h"
#include "../../Renderer/ModelViewLogic.h"

void RotationSystem::Start()
{
	Logger::Debug("Staring rotation system...");
	Registry->view<TransformComponent, RenderComponent>().each([](TransformComponent &transformComponent, RenderComponent &renderComponent)
	{
		ModelViewLogic::UpdateModelMatrix(renderComponent, transformComponent);
	});
}

void RotationSystem::Update(const float deltaTime)
{
	Registry->view<TransformComponent, RenderComponent>().each([&deltaTime](TransformComponent &transformComponent, RenderComponent &renderComponent)
	{
		TransformLogic::AdjustRotation(0.0f, 0.001f * deltaTime, 0.0f, transformComponent);
		ModelViewLogic::UpdateModelMatrix(renderComponent, transformComponent);
	});
}
