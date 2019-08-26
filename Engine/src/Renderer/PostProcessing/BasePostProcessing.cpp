#include "BasePostProcessing.h"
#include "../Shaders/VertexShader.h"
#include "../Shaders/PixelShader.h"
#include "../Generators/QuadGenerator.h"

BasePostProcessing::BasePostProcessing(ID3D11DeviceContext* deviceContext, ID3D11Device* device) : DeviceContext(deviceContext), Device(device)
{
	QuadGenerator::Generate(Device, VertexBufferInstance, IndexBufferInstance, -1.0f, -1.0f, 1.0f, 1.0f);
}

void BasePostProcessing::ChangeShader(const VertexShader& vertexShader, const PixelShader& pixelShader) const
{
	DeviceContext->IASetInputLayout(vertexShader.GetInputLayout());
	DeviceContext->VSSetShader(vertexShader.GetShader(), nullptr, 0);
	DeviceContext->PSSetShader(pixelShader.GetShader(), nullptr, 0);
}

void BasePostProcessing::InitializeOutputTextureMap(const int windowWidth, const int windowHeight, ID3D11Texture2D** renderTexture) const
{
	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	textureDesc.Width = windowWidth;
	textureDesc.Height = windowHeight;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	const auto hr = Device->CreateTexture2D(&textureDesc, nullptr, renderTexture);
	if (FAILED(hr))
		Logger::Error("Error creating output texture map!");
}

void BasePostProcessing::InitializeOutputRenderTargetView(const DXGI_FORMAT renderTargetFormat, ID3D11Texture2D* renderTexture,
	ID3D11RenderTargetView** renderTargetView) const
{
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	ZeroMemory(&renderTargetViewDesc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));

	renderTargetViewDesc.Format = renderTargetFormat;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	const auto hr = Device->CreateRenderTargetView(renderTexture, &renderTargetViewDesc, renderTargetView);
	if (FAILED(hr))
		Logger::Error("Error creating output render target view!");
}

void BasePostProcessing::InitializeShaderResourceView(const DXGI_FORMAT renderTargetFormat, ID3D11Texture2D* renderTexture,
	ID3D11ShaderResourceView** shaderResourceView) const
{
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	ZeroMemory(&shaderResourceViewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));

	shaderResourceViewDesc.Format = renderTargetFormat;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	const auto hr = Device->CreateShaderResourceView(renderTexture, &shaderResourceViewDesc, shaderResourceView);
	if (FAILED(hr))
		Logger::Error("Error creating intermediate shader resource view!");
}


void BasePostProcessing::UnbindShaderResourceViews(const int usedShaderResourceViews) const
{
	ID3D11ShaderResourceView* null = nullptr;
	for (auto i = 0; i < usedShaderResourceViews; i++)
		DeviceContext->PSSetShaderResources(i, 1, &null);
}