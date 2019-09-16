#include "ResourcesLoader.h"
#include "../Utils/Logger.h"
#include "MaterialLoader.h"
#include "ModelLoader.h"
#include <fstream>

void ResourcesLoader::Load(ID3D11Device* device, ID3D11DeviceContext* context, const std::string& pathToResourceFile)
{
	Logger::Debug("Preparing to load resource...");

	nlohmann::json jsonObject;
	std::ifstream inputFile(pathToResourceFile);
	inputFile >> jsonObject;

	LoadModels(device, jsonObject["Models"]);
	LoadTextures(device, context, jsonObject["Textures"]);
}

void ResourcesLoader::LoadModels(ID3D11Device* device, const nlohmann::json& json)
{
	for (auto it = json.begin(); it != json.end(); ++it)
	{
		const auto key = static_cast<std::string>(it.key());
		const auto value = it.value().get<std::string>();
		ModelLoader::LoadResource(device, value, key);
	}

	Logger::Debug("Loaded " + std::to_string(json.size()) + " model files.");
}

void ResourcesLoader::LoadTextures(ID3D11Device* device, ID3D11DeviceContext* context, const nlohmann::json& json)
{
	for (auto it = json.begin(); it != json.end(); ++it)
	{
		const auto id = static_cast<std::string>(it.key());
		auto value = it.value();

		const auto path = value["Path"].get<std::string>();
		const auto convertLatLongToCubeMap = value["ConvertLatLongToCubeMap"].is_null() ? false : value["ConvertLatLongToCubeMap"].get<bool>();

		MaterialLoader::LoadResource(device, context, path, id, convertLatLongToCubeMap);
	}

	Logger::Debug("Loaded " + std::to_string(json.size()) + " texture files.");
}

