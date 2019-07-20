#pragma once
#include <string>
#include <d3d11.h>
#include <json.hpp>

class ResourcesLoader final
{
public:
	static void Load(ID3D11Device* device, const std::string& pathToResourceFile);
private:
	static void LoadModels(ID3D11Device* device, const nlohmann::json& json);
	static void LoadTextures(ID3D11Device* device, const nlohmann::json& json);
};
