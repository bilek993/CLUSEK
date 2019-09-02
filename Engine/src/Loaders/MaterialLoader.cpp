#include "MaterialLoader.h"
#include <WICTextureLoader.h>
#include <DDSTextureLoader.h>
#include <fstream>
#include <json.hpp>
#include "../Utils/StringUtil.h"

// This variable cannot be inline due to stupid compiler error in VS 2017
std::unordered_map<std::string, std::shared_ptr<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>> MaterialLoader::TextureResources;

void MaterialLoader::LoadResource(ID3D11Device* device, const std::string& path, const std::string& resourceId)
{
	Logger::Debug("Preparing to load resource '" + resourceId + "' from path '" + path + "'...");
	const auto resource = std::make_shared<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>();

	if (path.empty())
		SetPinkTexture(device, *resource);
	else
		LoadTextureToMaterial(device, *resource, path);

	Logger::Debug("Adding resource '" + resourceId + "' into memory...");
	TextureResources[resourceId] = resource;
}

void MaterialLoader::SetResourceForMesh(ID3D11Device* device, Mesh& mesh, const std::string& albedoTextureId,
	const std::string& normalTextureId, const float alpha)
{
	mesh.Material.AlbedoTexture = GetTextureById(device, albedoTextureId);
	mesh.Material.NormalTexture = GetTextureById(device, normalTextureId);

	mesh.Material.Alpha = alpha;
}

void MaterialLoader::SetResourceForManuallyForSkyMaterial(ID3D11Device* device, SkyShaderMaterial& material,
	const std::string& albedoTextureId)
{
	material.SkyMap = GetTextureById(device, albedoTextureId);
}

void MaterialLoader::SetResourceForMeshGroup(ID3D11Device* device, std::vector<Mesh>& meshes, const std::string& pathToMaterial)
{
	if (pathToMaterial.empty())
	{
		Logger::Error("Incorrect path to material passed! Path cannot to material file cannot be empty.");
		return;
	}

	nlohmann::json jsonObject;
	std::ifstream inputFile(pathToMaterial);
	inputFile >> jsonObject;

	for (auto& mesh : meshes)
	{
		Logger::Debug("Preparing to load material '" + mesh.Name + "'...");
		auto alphaJsonInfo = jsonObject[mesh.Name]["Alpha"];
		auto albedoTextureJsonInfo = jsonObject[mesh.Name]["AlbedoTexture"];
		auto normalTextureJsonInfo = jsonObject[mesh.Name]["NormalTexture"];

		SetResourceForMesh(	device,
							mesh,
							albedoTextureJsonInfo.is_null() ? "" : albedoTextureJsonInfo.get<std::string>(),
							normalTextureJsonInfo.is_null() ? "" : normalTextureJsonInfo.get<std::string>(),
							alphaJsonInfo.is_null() ? 1.0f : alphaJsonInfo.get<float>());
	}
}

std::shared_ptr<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> MaterialLoader::GetTextureById(ID3D11Device* device, const std::string& id)
{
	const auto texturePointer = TextureResources.find(id);
	std::shared_ptr<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> outputTexture;

	if (id.empty())
	{
		Logger::Warning("Incorrect resource id.");
		outputTexture = std::make_shared<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>();
		SetPinkTexture(device, *outputTexture);
	}
	else if (texturePointer == TextureResources.end())
	{
		Logger::Warning("Resource with id '" + id + "' not found!");
		outputTexture = std::make_shared<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>();
		SetPinkTexture(device, *outputTexture);
	}
	else
	{
		outputTexture = texturePointer->second;
	}

	return outputTexture;
}

void MaterialLoader::LoadTextureToMaterial(ID3D11Device* device, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& textureResource, 
	const std::string& path)
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
