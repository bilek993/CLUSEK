#include "ResourcesGenerator.h"
#include "../../Utils/Logger.h"

ComputeTexture ResourcesGenerator::CreateCubeTexture(ID3D11Device* device, int width, int height, DXGI_FORMAT format,
	bool useMips)
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

ComputeTexture ResourcesGenerator::CreateFlatTexture(ID3D11Device* device, int width, int height, DXGI_FORMAT format,
	bool useMips)
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
	textureDesc.ArraySize = 1;
	textureDesc.Format = format;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	if (useMips)
	{
		textureDesc.BindFlags |= D3D11_BIND_RENDER_TARGET;
		textureDesc.MiscFlags |= D3D11_RESOURCE_MISC_GENERATE_MIPS;
	}

	auto hr = device->CreateTexture2D(&textureDesc, nullptr, computeTexture.Texture.GetAddressOf());
	if (FAILED(hr))
		Logger::Error("Failed to create flat texture!");

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	ZeroMemory(&shaderResourceViewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));

	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = -1;

	hr = device->CreateShaderResourceView(computeTexture.Texture.Get(), &shaderResourceViewDesc, computeTexture.ShaderResourceView.GetAddressOf());
	if (FAILED(hr))
		Logger::Error("Failed to create flat texture!");

	return computeTexture;
}

void ResourcesGenerator::CreateUnorderedAccessView(ID3D11Device* device, ComputeTexture& texture, const int mipSlice)
{
	texture.UnorderedAccessView.Reset();

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
