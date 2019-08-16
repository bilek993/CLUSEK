#include "MaterialLoader.h"
#include <WICTextureLoader.h>
#include <DDSTextureLoader.h>
#include <fstream>
#include <json.hpp>
#include "../Utils/StringUtil.h"

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

void MaterialLoader::SetResourceForMesh(ID3D11Device* device, Mesh& mesh, const std::string& albedoTextureId, const float alpha)
{
	const auto texturePointer = TextureResources.find(albedoTextureId);

	if (albedoTextureId.empty())
	{
		Logger::Warning("Incorrect resource id.");
		mesh.Material.AlbedoTexture = std::make_shared<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>();
		SetPinkTexture(device, *mesh.Material.AlbedoTexture);
	}
	else if (texturePointer == TextureResources.end())
	{
		Logger::Warning("Resource with id '" + albedoTextureId + "' not found!");
		mesh.Material.AlbedoTexture = std::make_shared<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>();
		SetPinkTexture(device, *mesh.Material.AlbedoTexture);
	}
	else
	{
		mesh.Material.AlbedoTexture = texturePointer->second;
	}

	mesh.Material.Alpha = alpha;
}

void MaterialLoader::SetResourceForManually(ID3D11Device* device, SkyShaderMaterial& material,
	const std::string& albedoTextureId)
{
	const auto texturePointer = TextureResources.find(albedoTextureId);

	if (albedoTextureId.empty())
	{
		Logger::Warning("Incorrect resource id.");
		material.SkyMap = std::make_shared<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>();
		SetPinkTexture(device, *material.SkyMap);
	}
	else if (texturePointer == TextureResources.end())
	{
		Logger::Warning("Resource with id '" + albedoTextureId + "' not found!");
		material.SkyMap = std::make_shared<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>();
		SetPinkTexture(device, *material.SkyMap);
	}
	else
	{
		material.SkyMap = texturePointer->second;
	}
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

		SetResourceForMesh(	device,
							mesh,
							albedoTextureJsonInfo.is_null() ? "" : albedoTextureJsonInfo.get<std::string>(),
							alphaJsonInfo.is_null() ? 1.0f : alphaJsonInfo.get<float>());
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
