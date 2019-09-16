#include "MaterialLoader.h"
#include <WICTextureLoader.h>
#include <DDSTextureLoader.h>
#include <fstream>
#include <json.hpp>
#include "../Utils/StringUtil.h"
#include "../Renderer/Shaders/ComputeShader.h"

// This variable cannot be inline due to stupid compiler error in VS 2017
std::unordered_map<std::string, std::shared_ptr<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>> MaterialLoader::TextureResources;

void MaterialLoader::LoadResource(ID3D11Device* device, ID3D11DeviceContext* context, const std::string& path, 
	const std::string& resourceId, const bool convertLatLongToCubeMap)
{
	Logger::Debug("Preparing to load resource '" + resourceId + "' from path '" + path + "'...");
	auto resource = std::make_shared<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>();

	if (!path.empty())
	{
		Logger::Debug("Adding resource '" + resourceId + "' into memory...");
		LoadTextureToMaterial(device, *resource, path);

		if (convertLatLongToCubeMap)
			resource = ConvertLatLongToCubeMap(device, context, resource->Get());

		TextureResources[resourceId] = resource;
	}
	else
	{
		Logger::Warning("Incorrect resource path (empty)!");
	}
}

void MaterialLoader::SetResourceForMesh(ID3D11Device* device, Mesh& mesh, const std::string& albedoTextureId,
	const std::string& normalTextureId, const std::string& metalicSmoothnessTextureId,
	const std::string& occlusionTextureId, const float alpha)
{
	mesh.Material.AlbedoTexture = GetTextureById(device, albedoTextureId, DefaultAlbedo);
	mesh.Material.NormalTexture = GetTextureById(device, normalTextureId, DefaultNormal);
	mesh.Material.MetalicSmoothnessTexture = GetTextureById(device, metalicSmoothnessTextureId, DefaultMetalicSmoothness);
	mesh.Material.OcclusionTexture = GetTextureById(device, occlusionTextureId, DefaultOcclusion);

	mesh.Material.Alpha = alpha;
}

void MaterialLoader::SetResourceForManuallyForSkyMaterial(ID3D11Device* device, SkyShaderMaterial& material,
	const std::string& albedoTextureId)
{
	material.SkyMap = GetTextureById(device, albedoTextureId, DefaultAlbedo);
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
		auto metalicSmoothnessTextureJsonInfo = jsonObject[mesh.Name]["MetalicSmoothnessTexture"];
		auto occlusionTextureJsonInfo = jsonObject[mesh.Name]["OcclusionTexture"];

		SetResourceForMesh(	device,
							mesh,
							albedoTextureJsonInfo.is_null() ? "" : albedoTextureJsonInfo.get<std::string>(),
							normalTextureJsonInfo.is_null() ? "" : normalTextureJsonInfo.get<std::string>(),
							metalicSmoothnessTextureJsonInfo.is_null() ? "" : metalicSmoothnessTextureJsonInfo.get<std::string>(),
							occlusionTextureJsonInfo.is_null() ? "" : occlusionTextureJsonInfo.get<std::string>(),
							alphaJsonInfo.is_null() ? 1.0f : alphaJsonInfo.get<float>());
	}
}

std::shared_ptr<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> MaterialLoader::GetTextureById(ID3D11Device* device, const std::string& id, 
	const FallbackColor fallbackColor)
{
	const auto texturePointer = TextureResources.find(id);
	std::shared_ptr<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> outputTexture;

	if (id.empty())
	{
		Logger::Debug("Resource not provided. Using fallback color texture.");
		outputTexture = std::make_shared<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>();
		SetDefaultTexture(device, *outputTexture, fallbackColor);
	}
	else if (texturePointer == TextureResources.end())
	{
		Logger::Warning("Resource with id '" + id + "' not found!");
		outputTexture = std::make_shared<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>();
		SetDefaultTexture(device, *outputTexture, fallbackColor);
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

void MaterialLoader::SetDefaultTexture(ID3D11Device* device, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& textureResource, const FallbackColor fallbackColor)
{
	CD3D11_TEXTURE2D_DESC textureDesc(DXGI_FORMAT_R8G8B8A8_UNORM, 1, 1);
	Microsoft::WRL::ComPtr<ID3D11Texture2D> p2DTexture = nullptr;
	D3D11_SUBRESOURCE_DATA initialData { &fallbackColor, sizeof(unsigned int), 0 };

	auto hr = device->CreateTexture2D(&textureDesc, &initialData, &p2DTexture);
	if (FAILED(hr))
		Logger::Error("Creating error texture failed!");

	const auto texture = static_cast<ID3D11Texture2D*>(p2DTexture.Get());
	CD3D11_SHADER_RESOURCE_VIEW_DESC srvDesc(D3D11_SRV_DIMENSION_TEXTURE2D, textureDesc.Format);

	hr = device->CreateShaderResourceView(texture, &srvDesc, textureResource.GetAddressOf());
	if (FAILED(hr))
		Logger::Error("Creating error texture failed!");
}

std::shared_ptr<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> MaterialLoader::ConvertLatLongToCubeMap(ID3D11Device* device, 
	ID3D11DeviceContext* context, ID3D11ShaderResourceView* inputResourceView)
{
	auto resource = std::make_shared<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>();

	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));

	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;
	const auto hr = device->CreateSamplerState(&samplerDesc, samplerState.GetAddressOf());
	if (FAILED(hr))
		Logger::Error("Couldn't create sampler state for 'Material Loader'!");

	// TODO: Add code here

	ID3D11UnorderedAccessView* const nullView[] = { nullptr };
	context->CSSetUnorderedAccessViews(0, 1, nullView, nullptr);

	return resource;
}
