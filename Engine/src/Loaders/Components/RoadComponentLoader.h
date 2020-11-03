#pragma once
#include "BaseComponentsLoader.h"
#include "../../ECS/Components/RoadComponent.h"

class RoadComponentLoader final : public BaseComponentsLoader
{
public:
	void Add(nlohmann::json& json, entt::registry& registry, const entt::registry::entity_type &entity) override;

private:
	void LoadPoints(RoadComponent& component, nlohmann::json& json) const;
};