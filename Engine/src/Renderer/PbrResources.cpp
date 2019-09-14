#include "PbrResources.h"
#include "../Utils/Logger.h"
#include "../Utils/StringUtil.h"
#include <DDSTextureLoader.h>
#include <WICTextureLoader.h>
#include "Shaders/ComputeShader.h"

bool PbrResource::Initialize(ID3D11Device* device, ID3D11DeviceContext* context, const std::string& pathToBrdfLutFile)
{
	Logger::Debug("Preparing PBR resources...");

	if (!LoadBrdfLutFile(device, pathToBrdfLutFile))
		return false;

	if (!GenerateIrradiance(device, context))
		return false;

	CleanUp(context);

	Logger::Debug("All PBR resources initialized!");
	return true;
}

ID3D11ShaderResourceView** PbrResource::GetAddressOfBrdfLutResourceTexture()
{
	return BrdfLutResourceTexture.GetAddressOf();
}

ID3D11ShaderResourceView** PbrResource::GetAddressOfIrradianceResourceTexture()
{
	return IrradianceTexture.ShaderResourceView.GetAddressOf();
}

bool PbrResource::LoadBrdfLutFile(ID3D11Device* device, const std::string& path)
{
	Logger::Debug("Preparing BRDF Lut file...");

	if (StringUtil::FindExtension(path) == "DDS")
	{
		const auto hr = DirectX::CreateDDSTextureFromFile(device, StringUtil::StringToWide(path).data(), nullptr, BrdfLutResourceTexture.GetAddressOf());
		if (FAILED(hr))
		{
			Logger::Error("Couldn't load texture from file!");
			return false;
		}
	}
	else
	{
		const auto hr = DirectX::CreateWICTextureFromFile(device, StringUtil::StringToWide(path).data(), nullptr, BrdfLutResourceTexture.GetAddressOf());
		if (FAILED(hr))
		{
			Logger::Error("Couldn't load texture from file!");
			return false;
		}
	}

	return true;
}

bool PbrResource::GenerateIrradiance(ID3D11Device* device, ID3D11DeviceContext* context)
{
	Logger::Debug("Preparing to generate irradiance map...");

	ComputeShader irradianceComputeShader;

	if (!irradianceComputeShader.Initialize(device, L"irradiance_compute_shader.cso"))
	{
		Logger::Error("IrradianceComputeShader not initialized due to critical problem!");
		return false;
	}

	IrradianceTexture = CreateCubeTexture(device, 32, 32, DXGI_FORMAT_R16G16B16A16_FLOAT, false);
	CreateUnorderedAccessView(device, IrradianceTexture);

	context->CSSetShader(irradianceComputeShader.GetShader(), nullptr, 0);
	context->CSSetUnorderedAccessViews(0, 1, IrradianceTexture.UnorderedAccessView.GetAddressOf(), nullptr);
	context->Dispatch(IrradianceTexture.Width / THREAD_COUNT, IrradianceTexture.Height / THREAD_COUNT, CUBE_SIZE);

	return true;
}

void PbrResource::CleanUp(ID3D11DeviceContext* context) const
{
	Logger::Debug("Preparing clean up mechanism, after PBR Resources initialization...");

	ID3D11UnorderedAccessView* const nullView[] = { nullptr };
	context->CSSetUnorderedAccessViews(0, 1, nullView, nullptr);
}

ComputeTexture PbrResource::CreateCubeTexture(ID3D11Device* device, const int width, const int height, 
	DXGI_FORMAT format, const bool useMips) const
{
	ComputeTexture computeTexture;
	computeTexture.Width = width;
	computeTexture.Height = height;
	computeTexture.Levels = 1;
	if (useMips)
	{
		while ((width | height) >> computeTexture.Levels)
			++computeTexture.Levels;
	}

	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.MipLevels = useMips ? 0 : 1;
	textureDesc.ArraySize = CUBE_SIZE;
	textureDesc.Format = format;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
	if (useMips)
	{
		textureDesc.BindFlags |= D3D11_BIND_RENDER_TARGET;
		textureDesc.MiscFlags |= D3D11_RESOURCE_MISC_GENERATE_MIPS;
	}

	auto hr = device->CreateTexture2D(&textureDesc, nullptr, computeTexture.Texture.GetAddressOf());
	if (FAILED(hr))
		Logger::Error("Failed to create cube texture!");

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	ZeroMemory(&shaderResourceViewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));

	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	shaderResourceViewDesc.TextureCube.MostDetailedMip = 0;
	shaderResourceViewDesc.TextureCube.MipLevels = -1;

	hr = device->CreateShaderResourceView(computeTexture.Texture.Get(), &shaderResourceViewDesc, computeTexture.ShaderResourceView.GetAddressOf());
	if (FAILED(hr))
		Logger::Error("Failed to create cube texture!");

	return computeTexture;
}

void PbrResource::CreateUnorderedAccessView(ID3D11Device* device, ComputeTexture& texture, const int mipSlice) const
{
	D3D11_TEXTURE2D_DESC textureDesc;
	texture.Texture->GetDesc(&textureDesc);

	D3D11_UNORDERED_ACCESS_VIEW_DESC unorderedAccessViewDesc;
	ZeroMemory(&unorderedAccessViewDesc, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));

	unorderedAccessViewDesc.Format = textureDesc.Format;
	unorderedAccessViewDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2DARRAY;
	unorderedAccessViewDesc.Texture2DArray.MipSlice = mipSlice;
	unorderedAccessViewDesc.Texture2DArray.FirstArraySlice = 0;
	unorderedAccessViewDesc.Texture2DArray.ArraySize = textureDesc.ArraySize;

	const auto hr = device->CreateUnorderedAccessView(texture.Texture.Get(), &unorderedAccessViewDesc, texture.UnorderedAccessView.GetAddressOf());
	if (FAILED(hr))
		Logger::Error("Failed to create unordered access view!");
}
