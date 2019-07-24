#include "RotationSystem.h"
#include "../Components/RenderComponent.h"
#include "../Components/TransformComponent.h"
#include "../../Renderer/TransformLogic.h"
#include "../../Renderer/ModelViewLogic.h"

void RotationSystem::Start(entt::registry& registry, const RenderWindow &window, const ConfigData& configData)
{
	Logger::Debug("Staring rotation system...");
	registry.view<TransformComponent, RenderComponent>().each([](TransformComponent &transformComponent, RenderComponent &renderComponent)
	{
		TransformLogic::SetPosition(0.0f, 0.0f, 0.0f, transformComponent);
		ModelViewLogic::UpdateModelMatrix(renderComponent, transformComponent);
	});
}

void RotationSystem::Update(float deltaTime, entt::registry& registry, IOData& ioData, IODevices& ioDevices,
	RenderWindow& window, ConfigData& configData, LightSettings &lightSettings)
{
	registry.view<TransformComponent, RenderComponent>().each([&deltaTime](TransformComponent &transformComponent, RenderComponent &renderComponent)
	{
		TransformLogic::AdjustRotation(0.0f, 0.001f * deltaTime, 0.0f, transformComponent);
		ModelViewLogic::UpdateModelMatrix(renderComponent, transformComponent);
	});
}
