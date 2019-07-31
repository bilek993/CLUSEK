#include "RenderComponentLoader.h"
#include "../../ECS/Components/RenderComponent.h"

void RenderComponentLoader::Add(const nlohmann::json& json, entt::registry& registry,
	const entt::registry::entity_type& entity)
{
	auto &component = registry.assign<RenderComponent>(entity);

	if (!json["ModelId"].is_null())
		component.ModelId = json["ModelId"].get<std::string>();
	if (!json["MaterialId"].is_null())
		component.MaterialId = json["MaterialId"].get<std::string>();
}
