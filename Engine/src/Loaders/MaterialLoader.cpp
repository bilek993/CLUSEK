#include "MaterialLoader.h"
#include <fstream>
#include <json.hpp>
#include "../Utils/StringUtil.h"

void MaterialLoader::LoadMaterialForMesh(ID3D11Device* device, Mesh& mesh, const std::wstring& pathToMainTexture)
{
	const auto hr = DirectX::CreateWICTextureFromFile(device, pathToMainTexture.data(), nullptr, mesh.Material.MainTexture.GetAddressOf());
	if (FAILED(hr))
		Logger::Error("Couldn't load example texture from file!");
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
			LoadMaterialForMesh(device, mesh, StringUtil::StringToWide(currentMaterialJsonInfo.get<std::string>()));
		}
		else
		{
			Logger::Warning("Material data for mesh '" + mesh.Name + "' doesn't exist in.");
			Logger::Warning("Using default material!");
		}
	}
}
