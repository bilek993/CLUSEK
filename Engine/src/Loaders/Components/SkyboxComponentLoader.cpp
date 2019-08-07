#include "SkyboxComponentLoader.h"
#include "../../ECS/Components/SkyboxComponent.h"

void SkyboxComponentLoader::Add(nlohmann::json& json, entt::registry& registry,
	const entt::registry::entity_type& entity)
{
	auto &component = registry.assign<SkyboxComponent>(entity);

	MAP_LOADER_BASIC_FIELD(ModelId, json, component, std::string);
	MAP_LOADER_BASIC_FIELD(SkyboxTextureId, json, component, std::string);
}
