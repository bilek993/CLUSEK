#include "RebuildSystem.h"

#include "../../Tags.h"

void RebuildSystem::Start()
{
}

void RebuildSystem::Rebuild()
{
	Logger::Debug("Cleaning up after rebuild...");

	const auto viewRequiresRebuildTag = Registry->view<entt::tag<Tags::REQUIRES_REBUILD>>();
	
	for (auto& entity : viewRequiresRebuildTag)
		Registry->remove<entt::tag<Tags::REQUIRES_REBUILD>>(entity);
}

void RebuildSystem::Update(float deltaTime)
{
}
