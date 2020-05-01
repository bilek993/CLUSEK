#include "TransformSystem.h"
#include "../Components/TransformComponent.h"
#include "../../Renderer/TransformLogic.h"

void TransformSystem::Start()
{
	Logger::Debug("Staring transform system...");

	Registry->view<TransformComponent>().each([](TransformComponent &transformComponent)
	{
		TransformLogic::SetPositionAndRotationEuler(	transformComponent.InitialPositionVector,
														transformComponent.InitialRotationVector, 
														transformComponent);
	});
}

void TransformSystem::Update(float deltaTime)
{
}
