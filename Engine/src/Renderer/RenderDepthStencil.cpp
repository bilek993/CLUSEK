#include "RenderDepthStencil.h"
#include "../Utils/Logger.h"

bool RenderDepthStencil::Initialize(ID3D11Device* device, 
									const int width, 
									const int height, 
									const int multisamplesCount,
                                    const int multisamplesQuality,
									const bool useShaderResourceView,
									const bool useDepthStencilState)
{
	Logger::Debug("Preparing to initialize render depth stencil...");

	if (!InitializeDepthStencilTextureBuffer(device, width, height, multisamplesCount, multisamplesQuality, useShaderResourceView))
		return false;

	if (!InitializeDepthStencilView(device))
		return false;

	if (useShaderResourceView)
	{
		if (!InitializeShaderResourceView(device))
			return false;
	}

	if (useDepthStencilState)
	{
		if (!InitializeDepthStencilState(device))
			return false;
	}

	Logger::Debug("Render depth stencil created without problems!");

	return true;
}

ID3D11Texture2D* RenderDepthStencil::GetDepthStencilTextureBufferPointer() const
{
	return DepthStencilTextureBuffer.Get();
}

ID3D11DepthStencilView* RenderDepthStencil::GetDepthStencilViewPointer() const
{
	return DepthStencilView.Get();
}

ID3D11DepthStencilState* RenderDepthStencil::GetDepthStencilStatePointer() const
{
	return DepthStencilState.Get();
}

bool RenderDepthStencil::InitializeDepthStencilTextureBuffer(	ID3D11Device* device, 
																const int width, 
																const int height, 
																const int multisamplesCount,
																const int multisamplesQuality,
																const bool useShaderResource)
{
	D3D11_TEXTURE2D_DESC depthStencilTextureDesc;
	ZeroMemory(&depthStencilTextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	unsigned int bindFlags = D3D11_BIND_DEPTH_STENCIL;
	if (useShaderResource)
		bindFlags |= D3D11_BIND_SHADER_RESOURCE;

	depthStencilTextureDesc.Width = width;
	depthStencilTextureDesc.Height = height;
	depthStencilTextureDesc.MipLevels = 1;
	depthStencilTextureDesc.ArraySize = 1;
	depthStencilTextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilTextureDesc.SampleDesc.Count = multisamplesCount;
	depthStencilTextureDesc.SampleDesc.Quality = multisamplesQuality;
	depthStencilTextureDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilTextureDesc.BindFlags = bindFlags;
	depthStencilTextureDesc.CPUAccessFlags = 0;
	depthStencilTextureDesc.MiscFlags = 0;

	const auto hr = device->CreateTexture2D(&depthStencilTextureDesc, nullptr, DepthStencilTextureBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		Logger::Error("Error creating depth stencil texture buffer!");
		return false;
	}

	return true;
}

bool RenderDepthStencil::InitializeDepthStencilView(ID3D11Device* device)
{
	const auto hr = device->CreateDepthStencilView(DepthStencilTextureBuffer.Get(), nullptr, DepthStencilView.GetAddressOf());
	if (FAILED(hr))
	{
		Logger::Error("Error creating depth stencil view!");
		return false;
	}

	return true;
}

bool RenderDepthStencil::InitializeShaderResourceView(ID3D11Device* device)
{
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	ZeroMemory(&shaderResourceViewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));

	shaderResourceViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	const auto hr = device->CreateShaderResourceView(DepthStencilTextureBuffer.Get(), &shaderResourceViewDesc, ShaderResourceView.GetAddressOf());
	if (FAILED(hr))
	{
		Logger::Error("Error creating shader resource view!");
		return false;
	}

	return true;
}

bool RenderDepthStencil::InitializeDepthStencilState(ID3D11Device* device)
{
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));

	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	const auto hr = device->CreateDepthStencilState(&depthStencilDesc, DepthStencilState.GetAddressOf());
	if (FAILED(hr))
	{
		Logger::Error("Error creating depth stencil state!");
		return false;
	}

	return true;
}
