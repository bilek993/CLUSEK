#include "PostProcessingLoader.h"
#include "../Utils/Logger.h"
#include <json.hpp>
#include <fstream>

void PostProcessingLoader::Load(const std::string& path, PostProcessingSettings* postProcessingSettings, 
	ID3D11DeviceContext* deviceContext, ID3D11Device* device, const int windowWidth, const int windowHeight)
{
	Logger::Debug("Preparing to load map from path '" + path + "'...");

	nlohmann::json jsonObject;
	std::ifstream inputFile(path);
	inputFile >> jsonObject;

	for (auto it = jsonObject.begin(); it != jsonObject.end(); ++it)
	{
		auto postProcessingEffectName = it.value().get<std::string>();

		MAP_POST_PROCESSING_EFFECT(postProcessingEffectName, "Gamma Correction", "gamma_correction_pixel_shader.cso");
		MAP_POST_PROCESSING_EFFECT(postProcessingEffectName, "Reinhard Tone Mapper", "reinhard_tone_mapper_pixel_shader.cso");
		MAP_POST_PROCESSING_EFFECT(postProcessingEffectName, "Simple ACES Tone Mapper", "simple_aces_tone_mapper_pixel_shader.cso");
		MAP_POST_PROCESSING_EFFECT(postProcessingEffectName, "Complex ACES Tone Mapper", "complex_aces_tone_mapper_pixel_shader.cso");
	}
}

void PostProcessingLoader::MapPostProcessingEffects(const std::string& effectInputName, const std::string& mappedEffectName,
	const std::string& pixelShaderFilename, DXGI_FORMAT format,	PostProcessingSettings* postProcessingSettings, 
	ID3D11DeviceContext* deviceContext, ID3D11Device* device, int windowWidth, int windowHeight)
{
	if (mappedEffectName == effectInputName)
	{
		Logger::Debug("Adding '" + effectInputName + "' post processing to stack...");
		postProcessingSettings->List.emplace_back(std::make_unique<ReusablePostProcessing>(deviceContext, device,
			windowWidth, windowHeight, format, effectInputName, pixelShaderFilename));
	}
}
