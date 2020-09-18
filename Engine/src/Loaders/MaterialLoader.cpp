#include "MaterialLoader.h"
#include <WICTextureLoader.h>
#include <DDSTextureLoader.h>
#include <fstream>
#include <json.hpp>
#include "../Utils/StringUtil.h"
#include "../Renderer/Shaders/ComputeShader.h"
#include "../Renderer/ComputeTexture.h"
#include "../Renderer/Generators/ResourcesGenerator.h"

// This variable cannot be inlined due to stupid compiler error in VS 2017
std::unordered_map<std::string, std::shared_ptr<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>> MaterialLoader::TextureResources;

void MaterialLoader::LoadResource(ID3D11Device* device, ID3D11DeviceContext* context, const std::string& path, const std::string& resourceId, 
	const std::string& convertLatLongToCubeMap, const std::string& srgbMode, const std::string& mipMaps, const ConfigData* config)
{
	Logger::Debug("Preparing to load resource '" + resourceId + "' from path '" + path + "'...");
	auto resource = std::make_shared<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>();

	const auto generateMipMaps = mipMaps == "GENERATE";
	const auto forceSrgb = srgbMode == "FORCED";
	const auto compatibleMode = convertLatLongToCubeMap == "COMPATIBLE";

	if (!path.empty())
	{
		Logger::Debug("Adding resource '" + resourceId + "' into memory...");
		LoadTextureToMaterial(device, context, *resource, path, forceSrgb, generateMipMaps);

		if (convertLatLongToCubeMap == "YES" || compatibleMode)
		{
			ContextMutex.lock();
			resource = ConvertLatLongToCubeMap(	device, 
												context, 
												resource->GetAddressOf(),
												config->CubemapGeneratedSize, 
												compatibleMode);
			ContextMutex.unlock();
		}

		ResourcesMapMutex.lock();
		TextureResources[resourceId] = resource;
		ResourcesMapMutex.unlock();
	}
	else
	{
		Logger::Warning("Incorrect resource path (empty)!");
	}
}

void MaterialLoader::SetResourceForMesh(ID3D11Device* device, Mesh& mesh, const std::string& albedoTextureId,
	const std::string& normalTextureId, const std::string& metalicSmoothnessTextureId,
	const std::string& occlusionTextureId, const std::string& emissionTextureId, const float alpha, const float thresholdAlpha,
	const bool twoSided, const float hightWindSpeed, const float hightWindScale, const float hightWindBase, const float localWindSpeed,
	const float localWindScale, const bool hightWindEnabled, const bool localWindEnabled)
{
	mesh.Material.AlbedoTexture = GetTextureById(device, albedoTextureId, DefaultAlbedo);
	mesh.Material.NormalTexture = GetTextureById(device, normalTextureId, DefaultNormal);
	mesh.Material.MetalicSmoothnessTexture = GetTextureById(device, metalicSmoothnessTextureId, DefaultMetalicSmoothness);
	mesh.Material.OcclusionTexture = GetTextureById(device, occlusionTextureId, DefaultOcclusion);
	mesh.Material.EmissionTexture = GetTextureById(device, emissionTextureId, DefaultEmission);

	mesh.Material.Alpha = alpha;
	mesh.Material.ThresholdAlpha = thresholdAlpha;
	mesh.Material.TwoSided = twoSided;
	mesh.Material.HightWindSpeed = hightWindSpeed;
	mesh.Material.HightWindScale = hightWindScale;
	mesh.Material.LocalWindSpeed = localWindSpeed;
	mesh.Material.LocalWindScale = localWindScale;
	mesh.Material.HightWindBase = hightWindBase;
	mesh.Material.HightWindEnabled = hightWindEnabled;
	mesh.Material.LocalWindEnabled = localWindEnabled;
}

void MaterialLoader::SetResourceForMeshGroup(ID3D11Device* device, std::vector<Mesh>& meshes, const std::string& pathToMaterial)
{
	if (pathToMaterial.empty())
	{
		Logger::Error("Incorrect path to material passed! Path to material file cannot be empty.");
		return;
	}

	nlohmann::json jsonObject;
	std::ifstream inputFile(pathToMaterial);
	inputFile >> jsonObject;

	for (auto& mesh : meshes)
	{
		Logger::Debug("Preparing to load material '" + mesh.Name + "'...");
		auto alphaJsonInfo = jsonObject[mesh.Name]["Alpha"];
		auto thresholdAlphaJsonInfo = jsonObject[mesh.Name]["ThresholdAlpha"];
		auto twoSidedJsonInfo = jsonObject[mesh.Name]["TwoSided"];
		auto hightWindSpeedJsonInfo = jsonObject[mesh.Name]["HightWindSpeed"];
		auto hightWindScaleJsonInfo = jsonObject[mesh.Name]["HightWindScale"];
		auto hightWindBaseJsonInfo = jsonObject[mesh.Name]["HightWindBase"];
		auto localWindSpeedJsonInfo = jsonObject[mesh.Name]["LocalWindSpeed"];
		auto localWindScaleJsonInfo = jsonObject[mesh.Name]["LocalWindScale"];
		auto hightWindEnabledJsonInfo = jsonObject[mesh.Name]["HightWindEnabled"];
		auto localWindEnabledJsonInfo = jsonObject[mesh.Name]["LocalWindEnabled"];
		auto albedoTextureJsonInfo = jsonObject[mesh.Name]["AlbedoTexture"];
		auto normalTextureJsonInfo = jsonObject[mesh.Name]["NormalTexture"];
		auto metalicSmoothnessTextureJsonInfo = jsonObject[mesh.Name]["MetalicSmoothnessTexture"];
		auto occlusionTextureJsonInfo = jsonObject[mesh.Name]["OcclusionTexture"];
		auto emissionTextureJsonInfo = jsonObject[mesh.Name]["EmissionTexture"];

		SetResourceForMesh(	device,
							mesh,
							albedoTextureJsonInfo.is_null() ? "" : albedoTextureJsonInfo.get<std::string>(),
							normalTextureJsonInfo.is_null() ? "" : normalTextureJsonInfo.get<std::string>(),
							metalicSmoothnessTextureJsonInfo.is_null() ? "" : metalicSmoothnessTextureJsonInfo.get<std::string>(),
							occlusionTextureJsonInfo.is_null() ? "" : occlusionTextureJsonInfo.get<std::string>(),
							emissionTextureJsonInfo.is_null() ? "" : emissionTextureJsonInfo.get<std::string>(),
							alphaJsonInfo.is_null() ? 1.0f : alphaJsonInfo.get<float>(), 
							thresholdAlphaJsonInfo.is_null() ? 0.0f : thresholdAlphaJsonInfo.get<float>(),
							twoSidedJsonInfo.is_null() ? false : twoSidedJsonInfo.get<bool>(),
							hightWindSpeedJsonInfo.is_null() ? 0.0f : hightWindSpeedJsonInfo.get<float>(),
							hightWindScaleJsonInfo.is_null() ? 0.0f : hightWindScaleJsonInfo.get<float>(),
							hightWindBaseJsonInfo.is_null() ? 0.0f : hightWindBaseJsonInfo.get<float>(),
							localWindSpeedJsonInfo.is_null() ? 0.0f : localWindSpeedJsonInfo.get<float>(),
							localWindScaleJsonInfo.is_null() ? 0.0f : localWindScaleJsonInfo.get<float>(),
							hightWindEnabledJsonInfo.is_null() ? 0.0f : hightWindEnabledJsonInfo.get<bool>(),
							localWindEnabledJsonInfo.is_null() ? 0.0f : localWindEnabledJsonInfo.get<bool>());
	}
}

void MaterialLoader::SetResourceForGrassMesh(ID3D11Device* device, GrassMesh& mesh, const std::string& albedoTextureId)
{
	mesh.Material.AlbedoTexture = GetTextureById(device, albedoTextureId, DefaultAlbedo);
}

void MaterialLoader::SetResourceForGrassMeshGroup(ID3D11Device* device, std::vector<GrassMesh>& meshes, const std::string& pathToMaterial)
{
	if (pathToMaterial.empty())
	{
		Logger::Error("Incorrect path to material passed! Path to material file cannot be empty.");
		return;
	}

	nlohmann::json jsonObject;
	std::ifstream inputFile(pathToMaterial);
	inputFile >> jsonObject;

	for (auto& mesh : meshes)
	{
		Logger::Debug("Preparing to load material '" + mesh.Name + "'...");
		auto albedoTextureJsonInfo = jsonObject[mesh.Name]["AlbedoTexture"];

		SetResourceForGrassMesh(device,
								mesh,
								albedoTextureJsonInfo.is_null() ? "" : albedoTextureJsonInfo.get<std::string>());
	}
}

void MaterialLoader::SetResourceForSkyMaterial(ID3D11Device* device, SkyShaderMaterial& material,
                                               const std::string& albedoTextureId)
{
	material.SkyMap = GetTextureById(device, albedoTextureId, DefaultAlbedo);
}

void MaterialLoader::SetResourceForTerrainMaterial(ID3D11Device* device, TerrainShaderMaterial& material,
	const std::string& pathToMaterial)
{
	nlohmann::json jsonObject;
	std::ifstream inputFile(pathToMaterial);
	inputFile >> jsonObject;

	auto heightmapJsonInfo = jsonObject["Heightmap"];
	auto splatmapJsonInfo = jsonObject["Splatmap"];
	auto grassPlacementJsonInfo = jsonObject["GrassPlacement"];

	auto texturesScaleJsonInfo = jsonObject["TexturesScale"];

	auto baseAlbedoTextureJsonInfo = jsonObject["BaseAlbedoTexture"];
	auto redAlbedoTextureJsonInfo = jsonObject["RedAlbedoTexture"];
	auto greenAlbedoTextureJsonInfo = jsonObject["GreenAlbedoTexture"];
	auto blueAlbedoTextureJsonInfo = jsonObject["BlueAlbedoTexture"];

	auto baseNormalTextureJsonInfo = jsonObject["BaseNormalTexture"];
	auto redNormalTextureJsonInfo = jsonObject["RedNormalTexture"];
	auto greenNormalTextureJsonInfo = jsonObject["GreenNormalTexture"];
	auto blueNormalTextureJsonInfo = jsonObject["BlueNormalTexture"];

	auto baseMetalicSmoothnessTextureJsonInfo = jsonObject["BaseMetalicSmoothnessTexture"];
	auto redMetalicSmoothnessTextureJsonInfo = jsonObject["RedMetalicSmoothnessTexture"];
	auto greenMetalicSmoothnessTextureJsonInfo = jsonObject["GreenMetalicSmoothnessTexture"];
	auto blueMetalicSmoothnessTextureJsonInfo = jsonObject["BlueMetalicSmoothnessTexture"];

	auto baseOcclusionTextureJsonInfo = jsonObject["BaseOcclusionTexture"];
	auto redOcclusionTextureJsonInfo = jsonObject["RedOcclusionTexture"];
	auto greenOcclusionTextureJsonInfo = jsonObject["GreenOcclusionTexture"];
	auto blueOcclusionTextureJsonInfo = jsonObject["BlueOcclusionTexture"];

	material.Heightmap = GetTextureById(device, heightmapJsonInfo.is_null() ? "" : heightmapJsonInfo.get<std::string>(), DefaultAlbedo);
	material.Splatmap = GetTextureById(device, splatmapJsonInfo.is_null() ? "" : splatmapJsonInfo.get<std::string>(), DefaultAlbedo);
	material.GrassPlacement = GetTextureById(device, grassPlacementJsonInfo.is_null() ? "" : grassPlacementJsonInfo.get<std::string>(), DefaultAlbedo);

	material.TexturesScale = texturesScaleJsonInfo.is_null() ? 1.0 : texturesScaleJsonInfo.get<float>();

	material.BaseAlbedoTexture = GetTextureById(device, baseAlbedoTextureJsonInfo.is_null() ? "" : baseAlbedoTextureJsonInfo.get<std::string>(), DefaultAlbedo);
	material.RedAlbedoTexture = GetTextureById(device, redAlbedoTextureJsonInfo.is_null() ? "" : redAlbedoTextureJsonInfo.get<std::string>(), DefaultAlbedo);
	material.GreenAlbedoTexture = GetTextureById(device, greenAlbedoTextureJsonInfo.is_null() ? "" : greenAlbedoTextureJsonInfo.get<std::string>(), DefaultAlbedo);
	material.BlueAlbedoTexture = GetTextureById(device, blueAlbedoTextureJsonInfo.is_null() ? "" : blueAlbedoTextureJsonInfo.get<std::string>(), DefaultAlbedo);

	material.BaseNormalTexture = GetTextureById(device, baseNormalTextureJsonInfo.is_null() ? "" : baseNormalTextureJsonInfo.get<std::string>(), DefaultNormal);
	material.RedNormalTexture = GetTextureById(device, redNormalTextureJsonInfo.is_null() ? "" : redNormalTextureJsonInfo.get<std::string>(), DefaultNormal);
	material.GreenNormalTexture = GetTextureById(device, greenNormalTextureJsonInfo.is_null() ? "" : greenNormalTextureJsonInfo.get<std::string>(), DefaultNormal);
	material.BlueNormalTexture = GetTextureById(device, blueNormalTextureJsonInfo.is_null() ? "" : blueNormalTextureJsonInfo.get<std::string>(), DefaultNormal);

	material.BaseMetalicSmoothnessTexture = GetTextureById(device, baseMetalicSmoothnessTextureJsonInfo.is_null() ? "" : baseMetalicSmoothnessTextureJsonInfo.get<std::string>(), DefaultMetalicSmoothness);
	material.RedMetalicSmoothnessTexture = GetTextureById(device, redMetalicSmoothnessTextureJsonInfo.is_null() ? "" : redMetalicSmoothnessTextureJsonInfo.get<std::string>(), DefaultMetalicSmoothness);
	material.GreenMetalicSmoothnessTexture = GetTextureById(device, greenMetalicSmoothnessTextureJsonInfo.is_null() ? "" : greenMetalicSmoothnessTextureJsonInfo.get<std::string>(), DefaultMetalicSmoothness);
	material.BlueMetalicSmoothnessTexture = GetTextureById(device, blueMetalicSmoothnessTextureJsonInfo.is_null() ? "" : blueMetalicSmoothnessTextureJsonInfo.get<std::string>(), DefaultMetalicSmoothness);

	material.BaseOcclusionTexture = GetTextureById(device, baseOcclusionTextureJsonInfo.is_null() ? "" : baseOcclusionTextureJsonInfo.get<std::string>(), DefaultOcclusion);
	material.RedOcclusionTexture = GetTextureById(device, redOcclusionTextureJsonInfo.is_null() ? "" : redOcclusionTextureJsonInfo.get<std::string>(), DefaultOcclusion);
	material.GreenOcclusionTexture = GetTextureById(device, greenOcclusionTextureJsonInfo.is_null() ? "" : greenOcclusionTextureJsonInfo.get<std::string>(), DefaultOcclusion);
	material.BlueOcclusionTexture = GetTextureById(device, blueOcclusionTextureJsonInfo.is_null() ? "" : blueOcclusionTextureJsonInfo.get<std::string>(), DefaultOcclusion);
}

void MaterialLoader::GetAndSetLoadingTexture(ID3D11Device* device, const std::string& path,
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& resourceView)
{
	Logger::Debug("Preparing to load loading logo from file...");

	if (StringUtil::FindExtension(path) == "DDS")
	{
		const auto hr = DirectX::CreateDDSTextureFromFile(device, StringUtil::StringToWide(path).data(), nullptr, resourceView.GetAddressOf());
		if (FAILED(hr))
			Logger::Error("Couldn't load texture from file!");
	}
	else
	{
		const auto hr = DirectX::CreateWICTextureFromFile(device, StringUtil::StringToWide(path).data(), nullptr, resourceView.GetAddressOf());
		if (FAILED(hr))
			Logger::Error("Couldn't load texture from file!");
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

void MaterialLoader::LoadTextureToMaterial(ID3D11Device* device, ID3D11DeviceContext* context, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& textureResource,
	const std::string& path, const bool forceSrgb, const bool generateMipMaps)
{
	const auto miscFlags = generateMipMaps ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0;
	UINT bindFlags = D3D11_BIND_SHADER_RESOURCE;
	if (generateMipMaps)
		bindFlags |= D3D11_BIND_RENDER_TARGET;

	if (generateMipMaps)
	{
		Logger::Debug("Preparing to lock mutex for loading texture and mip map generation...");
		ContextMutex.lock();
		Logger::Debug("Mutex locked for mip map creation...");
	}

	if (StringUtil::FindExtension(path) == "DDS")
	{
		const auto hr = DirectX::CreateDDSTextureFromFileEx(device,
															generateMipMaps ? context : nullptr,
															StringUtil::StringToWide(path).data(), 
															0, 
															D3D11_USAGE_DEFAULT,
															bindFlags,
															0, 
															miscFlags,
															forceSrgb, 
															nullptr, 
															textureResource.GetAddressOf(), 
															nullptr);
		if (FAILED(hr))
			Logger::Error("Couldn't load texture from file!");
	}
	else
	{
		const auto hr = DirectX::CreateWICTextureFromFileEx(device,
															generateMipMaps ? context : nullptr,
															StringUtil::StringToWide(path).data(), 
															0, 
															D3D11_USAGE_DEFAULT, 
															bindFlags, 
															0, 
															miscFlags, 
															forceSrgb ? DirectX::WIC_LOADER_FORCE_SRGB : DirectX::WIC_LOADER_DEFAULT, 
															nullptr, 
															textureResource.GetAddressOf());
		if (FAILED(hr))
			Logger::Error("Couldn't load texture from file!");
	}

	if (generateMipMaps)
	{
		ContextMutex.unlock();
		Logger::Debug("Mutex unlocked for mip map creation...");
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
	ID3D11DeviceContext* context, ID3D11ShaderResourceView* const* inputResourceView, const int textureSize, const bool compatibleMode)
{
	CreateSamplerStateIfNeeded(device);

	ComputeShader latlongToCubemapComputeShader;

	if (!latlongToCubemapComputeShader.Initialize(device, L"latlong_to_cubemap_compute_shader.cso"))
		Logger::Error("RadianceComputeShader not initialized due to critical problem!");

	auto textureFormat = DXGI_FORMAT_R16G16B16A16_FLOAT;

	if (compatibleMode)
	{
		Logger::Warning("You are using compatible mode for lat long converter! Consider switching to normal mode for better visual effects.");
		textureFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	}

	auto texture = ResourcesGenerator::CreateCubeTexture(device, textureSize, textureSize, textureFormat, !compatibleMode);
	ResourcesGenerator::CreateUnorderedAccessView(device, texture);

	context->CSSetShader(latlongToCubemapComputeShader.GetShader(), nullptr, 0);
	context->CSSetShaderResources(0, 1, inputResourceView);
	context->CSSetUnorderedAccessViews(0, 1, texture.UnorderedAccessView.GetAddressOf(), nullptr);
	context->CSSetSamplers(0, 1, SamplerState.GetAddressOf());
	context->Dispatch(texture.Width / THREAD_COUNT, texture.Height / THREAD_COUNT, CUBE_SIZE);

	context->GenerateMips(texture.ShaderResourceView.Get());

	ID3D11UnorderedAccessView* const nullView[] = { nullptr };
	context->CSSetUnorderedAccessViews(0, 1, nullView, nullptr);

	return std::make_shared<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>(texture.ShaderResourceView);
}

void MaterialLoader::CreateSamplerStateIfNeeded(ID3D11Device* device)
{
	if (SamplerState)
		return;

	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));

	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	const auto hr = device->CreateSamplerState(&samplerDesc, SamplerState.GetAddressOf());
	if (FAILED(hr))
		Logger::Error("Couldn't create sampler state for 'Material Loader'!");
}
