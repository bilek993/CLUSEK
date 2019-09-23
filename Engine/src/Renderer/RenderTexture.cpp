#include "RenderTexture.h"
#include "../Utils/Logger.h"

bool RenderTexture::Initialize(ID3D11Device* device, const int width, const int height, const DXGI_FORMAT renderTargetFormat,
	const int multisamplesCount, const int multisamplesQuality)
{
	const auto isMultiSamplingEnabled = multisamplesCount > 1;

	if (!InitializeRenderTexture2D(device, width, height, renderTargetFormat, multisamplesCount, multisamplesQuality))
		return false;

	if (!InitializeRenderTargetView(device, renderTargetFormat, isMultiSamplingEnabled))
		return false;

	if (!InitializeShaderResourceView(device, renderTargetFormat, isMultiSamplingEnabled))
		return false;

	return true;
}

bool RenderTexture::InitializeRenderTexture2D(ID3D11Device* device, const int width, const int height, const DXGI_FORMAT renderTargetFormat,
	const int multisamplesCount, const int multisamplesQuality)
{
	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = renderTargetFormat;
	textureDesc.SampleDesc.Count = multisamplesCount;
	textureDesc.SampleDesc.Quality = multisamplesQuality;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	const auto hr = device->CreateTexture2D(&textureDesc, nullptr, RenderTexture2D.GetAddressOf());
	if (FAILED(hr))
	{
		Logger::Error("Error creating 2D texture map for render texture!");
		return false;
	}

	return true;
}

bool RenderTexture::InitializeRenderTargetView(ID3D11Device* device, const DXGI_FORMAT renderTargetFormat,
	const bool multisampling)
{
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	ZeroMemory(&renderTargetViewDesc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));

	renderTargetViewDesc.Format = renderTargetFormat;
	renderTargetViewDesc.ViewDimension = multisampling ? D3D11_RTV_DIMENSION_TEXTURE2DMS : D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	const auto hr = device->CreateRenderTargetView(RenderTexture2D.Get(), &renderTargetViewDesc, RenderTargetView.GetAddressOf());
	if (FAILED(hr))
	{
		Logger::Error("Error creating render target view for render texture!");
		return false;
	}

	return true;
}

bool RenderTexture::InitializeShaderResourceView(ID3D11Device* device, const DXGI_FORMAT renderTargetFormat,
	const bool multisampling)
{
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	ZeroMemory(&shaderResourceViewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));

	shaderResourceViewDesc.Format = renderTargetFormat;
	shaderResourceViewDesc.ViewDimension = multisampling ? D3D11_SRV_DIMENSION_TEXTURE2DMS : D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	const auto hr = device->CreateShaderResourceView(RenderTexture2D.Get(), &shaderResourceViewDesc, ShaderResourceView.GetAddressOf());
	if (FAILED(hr))
	{
		Logger::Error("Error creating shader resource view for render texture!");
		return false;
	}

	return true;
}

ID3D11RenderTargetView* const* RenderTexture::GetAddressOfRenderTargetView() const
{
	return RenderTargetView.GetAddressOf();
}

ID3D11RenderTargetView* RenderTexture::GetRenderTargetViewPointer() const
{
	return RenderTargetView.Get();
}

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> RenderTexture::GetShaderResourceView() const
{
	return ShaderResourceView;
}
