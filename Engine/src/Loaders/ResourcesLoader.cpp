#include "ResourcesLoader.h"
#include "../Utils/Logger.h"
#include "MaterialLoader.h"
#include "ModelLoader.h"
#include <fstream>

void ResourcesLoader::Load(ID3D11Device* device, const std::string& pathToResourceFile)
{
	Logger::Debug("Preparing to load resource...");

	nlohmann::json jsonObject;
	std::ifstream inputFile(pathToResourceFile);
	inputFile >> jsonObject;

	LoadModels(device, jsonObject["Models"]);
	LoadTextures(device, jsonObject["Textures"]);
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

void ResourcesLoader::LoadTextures(ID3D11Device* device, const nlohmann::json& json)
{
	for (auto it = json.begin(); it != json.end(); ++it)
	{
		const auto key = static_cast<std::string>(it.key());
		const auto value = it.value()["Path"].get<std::string>();
		MaterialLoader::LoadResource(device, value, key);
	}

	Logger::Debug("Loaded " + std::to_string(json.size()) + " texture files.");
}

