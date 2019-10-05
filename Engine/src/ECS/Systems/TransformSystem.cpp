#include "TransformSystem.h"

void TransformSystem::Start()
{
	Logger::Debug("Staring transform system...");
	RecalculateAll();
}

void TransformSystem::Update(float deltaTime)
{
	RecalculateAll();
}

void TransformSystem::RecalculateAll() const
{
	Recalculate<ModelRenderComponent>();
	Recalculate<SkyboxComponent>();
}
