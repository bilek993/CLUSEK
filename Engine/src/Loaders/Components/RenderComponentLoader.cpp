#include "RenderComponentLoader.h"
#include "../../ECS/Components/RenderComponent.h"

void RenderComponentLoader::Add(const nlohmann::json& json, entt::registry& registry,
	const entt::registry::entity_type& entity)
{
	registry.assign<RenderComponent>(entity, "Nanosuit", "Data/Models/Nanosuit/nanosuit-material.json");
}
