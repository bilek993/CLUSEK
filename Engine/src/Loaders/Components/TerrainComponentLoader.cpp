#include "TerrainComponentLoader.h"
#include "../../ECS/Components/TerrainComponent.h"

void TerrainComponentLoader::Add(nlohmann::json& json, entt::registry& registry,
	const entt::registry::entity_type& entity)
{
	auto &component = registry.assign<TerrainComponent>(entity);

	MAP_LOADER_BASIC_FIELD(PathToHeightmap, json, component, std::string);
	MAP_LOADER_BASIC_FIELD(MaterialId, json, component, std::string);
	MAP_LOADER_BASIC_FIELD(QualityDivider, json, component, int);
	MAP_LOADER_BASIC_FIELD(ScaleXZ, json, component, float);
	MAP_LOADER_BASIC_FIELD(MaxHeight, json, component, float);
}
