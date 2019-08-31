#pragma once
#include <string>
#include "../Renderer/PostProcessingSettings.h"

// This macro is designed for simpler PostProcessingLoader::MapPostProcessingEffects method calls.
#define MAP_POST_PROCESSING_EFFECT(INPUT_NAME, MAPPED_NAME, VERTEX_FILENAME, PIXEL_FILENAME) MapPostProcessingEffects(INPUT_NAME, MAPPED_NAME, VERTEX_FILENAME, PIXEL_FILENAME, postProcessingSettings->Format, postProcessingSettings, deviceContext, device, windowWidth, windowHeight);

class PostProcessingLoader final
{
public:
	static void Load(const std::string& path, PostProcessingSettings* postProcessingSettings, 
		ID3D11DeviceContext* deviceContext, ID3D11Device* device, int windowWidth, int windowHeight);
private:
	static inline void MapPostProcessingEffects(const std::string& effectInputName, const std::string& mappedEffectName, 
		const std::string& pixelShaderFilename, const std::string&  vertexShaderFilename, DXGI_FORMAT format, 
		PostProcessingSettings* postProcessingSettings,	ID3D11DeviceContext* deviceContext, ID3D11Device* device, 
		int windowWidth, int windowHeight);
};
