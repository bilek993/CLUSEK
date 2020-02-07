#include "TransformSystem.h"
#include "../Components/TerrainComponent.h"

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
	Recalculate<TerrainComponent>();
}
