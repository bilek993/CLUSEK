#include "ModelRenderComponentLoader.h"
#include "../../ECS/Components/ModelRenderComponent.h"

void ModelRenderComponentLoader::Add(nlohmann::json& json, entt::registry& registry,
	const entt::registry::entity_type& entity)
{
	auto &component = registry.assign<ModelRenderComponent>(entity);

	MAP_LOADER_BASIC_FIELD(Invisible, json, component, bool);
	MAP_LOADER_BASIC_FIELD(LowPolyDistance, json, component, float);
	MAP_LOADER_BASIC_FIELD(LodTransitionDistance, json, component, float);
	MAP_LOADER_BASIC_FIELD(ModelId, json, component, std::string);
	MAP_LOADER_BASIC_FIELD(LowPolyModelId, json, component, std::string);
	MAP_LOADER_BASIC_FIELD(MaterialId, json, component, std::string);
}
