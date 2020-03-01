#include "RigidbodyStaticHeightFieldsComponentLoader.h"
#include "../../ECS/Components/RigidbodyStaticHeightFieldsComponent.h"

void RigidbodyStaticHeightFieldsComponentLoader::Add(nlohmann::json& json, entt::registry& registry,
	const entt::registry::entity_type& entity)
{
	auto &component = registry.assign<RigidbodyStaticHeightFieldsComponent>(entity);
}
