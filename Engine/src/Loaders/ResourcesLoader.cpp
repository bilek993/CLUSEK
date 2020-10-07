#include "ResourcesLoader.h"
#include "../Utils/Logger.h"
#include "MaterialLoader.h"
#include "ModelLoader.h"
#include <fstream>
#include <future>

void ResourcesLoader::LoadFromRenderer(ID3D11Device* device, ID3D11DeviceContext* context, const ConfigData* config)
{
	Logger::Debug("Preparing to load resources...");

	nlohmann::json jsonObject;
	std::ifstream inputFile(config->PathToResources);
	inputFile >> jsonObject;

	LoadModels(device, jsonObject["Models"], config);
	Logger::Debug("Loaded all model files.");

	LoadGrassModels(device, jsonObject["GrassModels"], config);
	Logger::Debug("Loaded all grass model files.");

	LoadTextures(device, context, jsonObject["Textures"], config);
	Logger::Debug("Loaded all texture files.");
}

void ResourcesLoader::LoadFromPhysics(physx::PxPhysics& physics, const physx::PxCooking& cooking, const ConfigData* config)
{
	Logger::Debug("Preparing to load resources...");

	nlohmann::json jsonObject;
	std::ifstream inputFile(config->PathToResources);
	inputFile >> jsonObject;

	LoadConvexModels(physics, cooking, jsonObject["ConvexModels"], config);
	Logger::Debug("Loaded all convex model files.");
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
}

void ResourcesLoader::LoadGrassModels(ID3D11Device* device, const nlohmann::json& json, const ConfigData* config)
{
	std::vector<std::future<void>> asyncFutures;

	for (auto it = json.begin(); it != json.end(); ++it)
	{
		const auto key = static_cast<std::string>(it.key());
		const auto value = it.value().get<std::string>();

		if (config->EnableAsyncModelLoading)
			asyncFutures.emplace_back(std::async(std::launch::async, ModelLoader::LoadGrassResource, device, value, key));
		else
			ModelLoader::LoadGrassResource(device, value, key);
	}
}

void ResourcesLoader::LoadConvexModels(physx::PxPhysics& physics, const physx::PxCooking& cooking, const nlohmann::json& json,
	const ConfigData* config)
{
	std::vector<std::future<void>> asyncFutures;

	for (auto it = json.begin(); it != json.end(); ++it)
	{
		const auto key = static_cast<std::string>(it.key());
		const auto value = it.value().get<std::string>();

		if (config->EnableAsyncModelLoading)
			asyncFutures.emplace_back(std::async(std::launch::async, ModelLoader::LoadConvexResource, &physics, &cooking, value, key));
		else
			ModelLoader::LoadConvexResource(&physics, &cooking, value, key);
	}
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
		const auto srgbMode = value["SRGBMode"].is_null() ? "DEFAULT" : value["SRGBMode"].get<std::string>();
		const auto mipMaps = value["MipMaps"].is_null() ? "SKIP" : value["MipMaps"].get<std::string>();

		if (config->EnableAsyncTextureLoading)
		{
			asyncFutures.emplace_back(	std::async(std::launch::async, 
										MaterialLoader::LoadResource, 
										device, 
										context, 
										path, 
										id, 
										convertLatLongToCubeMap, 
										srgbMode, 
										mipMaps, 
										config));
		}
		else
		{
			MaterialLoader::LoadResource(	device, 
											context, 
											path, 
											id, 
											convertLatLongToCubeMap, 
											srgbMode, 
											mipMaps, 
											config);
		}
	}
}

