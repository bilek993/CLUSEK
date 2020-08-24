#include "GrassComponentLoader.h"
#include "../../ECS/Components/GrassComponent.h"

void GrassComponentLoader::Add(nlohmann::json& json, entt::registry& registry, const entt::registry::entity_type& entity)
{
	auto &component = registry.assign<GrassComponent>(entity);

	MAP_LOADER_BASIC_FIELD(AlphaDiscardPoint, json, component, float);
	MAP_LOADER_BASIC_FIELD(AlbedoInterpolationRatio, json, component, float);
	MAP_LOADER_BASIC_FIELD(NormalInterpolationRatio, json, component, float);
	MAP_LOADER_BASIC_FIELD(RoughnessInterpolationTarget, json, component, float);
	MAP_LOADER_BASIC_FIELD(RoughnessInterpolationRatio, json, component, float);
	MAP_LOADER_BASIC_FIELD(MetalicInterpolationTarget, json, component, float);
	MAP_LOADER_BASIC_FIELD(MetalicInterpolationRatio, json, component, float);
	MAP_LOADER_BASIC_FIELD(OcclusionValue, json, component, float);
	MAP_LOADER_BASIC_FIELD(WindVertexAffectHeight, json, component, float);
	MAP_LOADER_BASIC_FIELD(GlobalWindSpeed, json, component, float);
	MAP_LOADER_BASIC_FIELD(GlobalWindScale, json, component, float);
	MAP_LOADER_BASIC_FIELD(LocalWindSpeed, json, component, float);

	if (!json["LocalWindScaleX"].is_null() && !json["LocalWindScaleY"].is_null() && !json["LocalWindScaleZ"].is_null())
	{
		const auto x = json["LocalWindScaleX"].get<float>();
		const auto y = json["LocalWindScaleY"].get<float>();
		const auto z = json["LocalWindScaleZ"].get<float>();

		component.LocalWindScale = DirectX::XMFLOAT3(x, y, z);
	}
	
	MAP_LOADER_BASIC_FIELD(ModelId, json, component, std::string);
	MAP_LOADER_BASIC_FIELD(MaterialId, json, component, std::string);
}
