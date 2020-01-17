#include "ResourcesLoader.h"
#include "../Utils/Logger.h"
#include "MaterialLoader.h"
#include "ModelLoader.h"
#include <fstream>
#include <future>

void ResourcesLoader::Load(ID3D11Device* device, ID3D11DeviceContext* context, const ConfigData* config)
{
	Logger::Debug("Preparing to load resource...");

	nlohmann::json jsonObject;
	std::ifstream inputFile(config->PathToResources);
	inputFile >> jsonObject;

	LoadModels(device, jsonObject["Models"], config);
	LoadTextures(device, context, jsonObject["Textures"], config);
}

void ResourcesLoader::LoadModels(ID3D11Device* device, const nlohmann::json& json, const ConfigData* config)
{
	std::vector<std::future<void>> asyncFutures;

	for (auto it = json.begin(); it != json.end(); ++it)
	{
		const auto key = static_cast<std::string>(it.key());
		const auto value = it.value().get<std::string>();

		if (config->EnableAsyncModelLoading)
			asyncFutures.emplace_back(std::async(std::launch::async, ModelLoader::LoadResource, device, value, key));
		else
			ModelLoader::LoadResource(device, value, key);
	}

	Logger::Debug("Loaded " + std::to_string(json.size()) + " model files.");
}

void ResourcesLoader::LoadTextures(ID3D11Device* device, ID3D11DeviceContext* context, const nlohmann::json& json,
	const ConfigData* config)
{
	std::vector<std::future<void>> asyncFutures;

	for (auto it = json.begin(); it != json.end(); ++it)
	{
		const auto id = static_cast<std::string>(it.key());
		auto value = it.value();

		const auto path = value["Path"].get<std::string>();
		const auto convertLatLongToCubeMap = value["ConvertLatLongToCubeMap"].is_null() ? "NO" : value["ConvertLatLongToCubeMap"].get<std::string>();

		if (config->EnableAsyncTextureLoading)
			asyncFutures.emplace_back(std::async(std::launch::async, MaterialLoader::LoadResource, device, context, path, id, convertLatLongToCubeMap, config));
		else
			MaterialLoader::LoadResource(device, context, path, id, convertLatLongToCubeMap, config);
	}

	Logger::Debug("Loaded " + std::to_string(json.size()) + " texture files.");
}

