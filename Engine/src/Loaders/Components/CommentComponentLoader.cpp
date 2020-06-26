#include "CommentComponentLoader.h"
#include "../../ECS/Components/CommentComponent.h"

void CommentComponentLoader::Add(nlohmann::json& json, entt::registry& registry,
	const entt::registry::entity_type& entity)
{
	auto &component = registry.assign<CommentComponent>(entity);

	MAP_LOADER_BASIC_FIELD(Comment, json, component, std::string);
}
