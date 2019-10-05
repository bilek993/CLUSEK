#include "TransformSystem.h"
#include "../Components/TransformComponent.h"
#include "../Components/ModelRenderComponent.h"
#include "../../Renderer/ModelViewLogic.h"
#include "../Components/SkyboxComponent.h"

void TransformSystem::Start()
{
}

void TransformSystem::Update(float deltaTime)
{
	Recalculate();
}

void TransformSystem::Recalculate() const
{
	Registry->view<TransformComponent, ModelRenderComponent>().each([](TransformComponent &transformComponent, ModelRenderComponent &modelRenderComponent)
	{
		if (transformComponent.ValuesChanged)
		{
			transformComponent.ValuesChanged = false;
			ModelViewLogic::UpdateModelMatrix(modelRenderComponent.WorldMatrix, transformComponent);
		}
	});

	Registry->view<TransformComponent, SkyboxComponent>().each([](TransformComponent &transformComponent, SkyboxComponent &skyboxComponent)
	{
		if (transformComponent.ValuesChanged)
		{
			transformComponent.ValuesChanged = false;
			ModelViewLogic::UpdateModelMatrix(skyboxComponent.WorldMatrix, transformComponent);
		}
	});
}
