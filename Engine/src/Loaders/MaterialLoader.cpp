#include "MaterialLoader.h"
#include <fstream>
#include <json.hpp>
#include "../Utils/StringUtil.h"

void MaterialLoader::LoadMaterialForMesh(ID3D11Device* device, Mesh& mesh, const std::string& pathToMainTexture)
{
	LoadTextureToMaterial(device, mesh.Material.MainTexture, pathToMainTexture);
}

void MaterialLoader::LoadMaterialForMeshGroup(ID3D11Device* device, std::vector<Mesh>& meshes, const std::string& pathToMaterial)
{
	nlohmann::json jsonObject;
	std::ifstream inputFile(pathToMaterial);
	inputFile >> jsonObject;

	for (auto& mesh : meshes)
	{
		auto currentMaterialJsonInfo = jsonObject[mesh.Name]["MainTexture"];

		if (!currentMaterialJsonInfo.is_null())
		{
			Logger::Debug("Preparing to load material '" + mesh.Name + "'...");
			LoadMaterialForMesh(device, mesh, currentMaterialJsonInfo.get<std::string>());
		}
		else
		{
			Logger::Warning("Material data for mesh '" + mesh.Name + "' doesn't exist in.");
			Logger::Warning("Using default material!");
		}
	}
}

void MaterialLoader::LoadTextureToMaterial(ID3D11Device* device, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& textureResource, const std::string& path)
{
	if (StringUtil::FindExtension(path) == "DDS")
	{
		const auto hr = DirectX::CreateDDSTextureFromFile(device, StringUtil::StringToWide(path).data(), nullptr, textureResource.GetAddressOf());
		if (FAILED(hr))
			Logger::Error("Couldn't load example texture from file!");
	}
	else
	{
		const auto hr = DirectX::CreateWICTextureFromFile(device, StringUtil::StringToWide(path).data(), nullptr, textureResource.GetAddressOf());
		if (FAILED(hr))
			Logger::Error("Couldn't load example texture from file!");
	}
}
