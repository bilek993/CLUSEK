#include "MaterialLoader.h"
#include <WICTextureLoader.h>
#include <DDSTextureLoader.h>
#include <fstream>
#include <json.hpp>
#include "../Utils/StringUtil.h"

void MaterialLoader::LoadMaterialForMesh(ID3D11Device* device, Mesh& mesh, const std::string& pathToMainTexture)
{
	if (pathToMainTexture.empty())
		SetPinkTexture(device, mesh.Material.MainTexture);
	else
		LoadTextureToMaterial(device, mesh.Material.MainTexture, pathToMainTexture);
}

void MaterialLoader::LoadMaterialForMeshGroup(ID3D11Device* device, std::vector<Mesh>& meshes, const std::string& pathToMaterial)
{
	nlohmann::json jsonObject;
	std::ifstream inputFile(pathToMaterial);
	inputFile >> jsonObject;

	for (auto& mesh : meshes)
	{
		Logger::Debug("Preparing to load material '" + mesh.Name + "'...");
		auto currentMaterialJsonInfo = jsonObject[mesh.Name]["MainTexture"];

		LoadMaterialForMesh(device,
							mesh,
							currentMaterialJsonInfo.is_null() ? "" : currentMaterialJsonInfo.get<std::string>());
	}
}

void MaterialLoader::LoadTextureToMaterial(ID3D11Device* device, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& textureResource, const std::string& path)
{
	if (StringUtil::FindExtension(path) == "DDS")
	{
		const auto hr = DirectX::CreateDDSTextureFromFile(device, StringUtil::StringToWide(path).data(), nullptr, textureResource.GetAddressOf());
		if (FAILED(hr))
			Logger::Error("Couldn't load texture from file!");
	}
	else
	{
		const auto hr = DirectX::CreateWICTextureFromFile(device, StringUtil::StringToWide(path).data(), nullptr, textureResource.GetAddressOf());
		if (FAILED(hr))
			Logger::Error("Couldn't load texture from file!");
	}
}

void MaterialLoader::SetPinkTexture(ID3D11Device* device, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& textureResource)
{
	Logger::Warning("Texture not found! Pink texture will be used instead.");

	CD3D11_TEXTURE2D_DESC textureDesc(DXGI_FORMAT_R8G8B8A8_UNORM, 1, 1);
	Microsoft::WRL::ComPtr<ID3D11Texture2D> p2DTexture = nullptr;
	D3D11_SUBRESOURCE_DATA initialData { &PINK_COLOR, sizeof(unsigned int), 0 };

	auto hr = device->CreateTexture2D(&textureDesc, &initialData, &p2DTexture);
	if (FAILED(hr))
		Logger::Error("Creating error texture failed!");

	const auto texture = static_cast<ID3D11Texture2D*>(p2DTexture.Get());
	CD3D11_SHADER_RESOURCE_VIEW_DESC srvDesc(D3D11_SRV_DIMENSION_TEXTURE2D, textureDesc.Format);

	hr = device->CreateShaderResourceView(texture, &srvDesc, textureResource.GetAddressOf());
	if (FAILED(hr))
		Logger::Error("Creating error texture failed!");
}
