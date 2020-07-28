#include "GrassComponentLoader.h"
#include "../../ECS/Components/GrassComponent.h"

void GrassComponentLoader::Add(nlohmann::json& json, entt::registry& registry, const entt::registry::entity_type& entity)
{
	auto &component = registry.assign<GrassComponent>(entity);

	MAP_LOADER_BASIC_FIELD(ModelId, json, component, std::string);
	MAP_LOADER_BASIC_FIELD(MaterialId, json, component, std::string);
}
