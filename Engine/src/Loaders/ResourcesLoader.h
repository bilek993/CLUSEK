#pragma once
#include <string>
#include <d3d11.h>
#include <json.hpp>
#include "ConfigData.h"

class ResourcesLoader final
{
public:
	static void Load(ID3D11Device* device, ID3D11DeviceContext* context, const ConfigData* config);
	
private:
	static void LoadModels(ID3D11Device* device, const nlohmann::json& json, const ConfigData* config);
	static void LoadGrassModels(ID3D11Device* device, const nlohmann::json& json, const ConfigData* config);
	
	static void LoadTextures(ID3D11Device* device, ID3D11DeviceContext* context, const nlohmann::json& json,
		const ConfigData* config);
};
