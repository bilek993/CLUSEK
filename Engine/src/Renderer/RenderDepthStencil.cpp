#include "RenderDepthStencil.h"
#include "../Utils/Logger.h"

bool RenderDepthStencil::Initialize(ID3D11Device* device, 
									const int width,
									const int height,
									const DXGI_FORMAT formatTexture,
									const DXGI_FORMAT formatDepthStencilView,
									const DXGI_FORMAT formatShaderResourceView,
									const int multisamplesCount,
                                    const int multisamplesQuality,
									const bool useShaderResourceView,
									const bool useDepthStencilState,
									const int numberOfTextures)
{
	Logger::Debug("Preparing to initialize render depth stencil...");

	const auto isMultisamplingEnabled = multisamplesCount > 1;

	if (!InitializeDepthStencilTextureBuffer(	device,
												width,
												height,
												formatTexture,
												multisamplesCount,
												multisamplesQuality,
												useShaderResourceView,
												numberOfTextures))
		return false;

	if (!InitializeDepthStencilView(device, formatDepthStencilView, isMultisamplingEnabled))
		return false;

	if (useShaderResourceView)
	{
		if (!InitializeShaderResourceView(device, formatShaderResourceView, isMultisamplingEnabled))
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

ID3D11ShaderResourceView* const* RenderDepthStencil::GetAddressOfShaderResourceView() const
{
	return ShaderResourceView.GetAddressOf();
}

ID3D11ShaderResourceView* RenderDepthStencil::GetShaderResourceViewPointer() const
{
	return ShaderResourceView.Get();
}

ID3D11DepthStencilState* RenderDepthStencil::GetDepthStencilStatePointer() const
{
	return DepthStencilState.Get();
}

bool RenderDepthStencil::InitializeDepthStencilTextureBuffer(	ID3D11Device* device, 
																const int width, 
																const int height,
																const DXGI_FORMAT format,
																const int multisamplesCount,
																const int multisamplesQuality,
																const bool useShaderResourceView,
																const int numberOfTextures)
{
	D3D11_TEXTURE2D_DESC depthStencilTextureDesc;
	ZeroMemory(&depthStencilTextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	unsigned int bindFlags = D3D11_BIND_DEPTH_STENCIL;
	if (useShaderResourceView)
		bindFlags |= D3D11_BIND_SHADER_RESOURCE;

	depthStencilTextureDesc.Width = width;
	depthStencilTextureDesc.Height = height;
	depthStencilTextureDesc.MipLevels = 1;
	depthStencilTextureDesc.ArraySize = numberOfTextures;
	depthStencilTextureDesc.Format = format;
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

bool RenderDepthStencil::InitializeDepthStencilView(ID3D11Device* device, const DXGI_FORMAT format, const bool multisamplingEnabled)
{
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));

	depthStencilViewDesc.Format = format;
	depthStencilViewDesc.ViewDimension = multisamplingEnabled ? D3D11_DSV_DIMENSION_TEXTURE2DMS :  D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	const auto hr = device->CreateDepthStencilView(DepthStencilTextureBuffer.Get(), &depthStencilViewDesc, DepthStencilView.GetAddressOf());
	if (FAILED(hr))
	{
		Logger::Error("Error creating depth stencil view!");
		return false;
	}

	return true;
}

bool RenderDepthStencil::InitializeShaderResourceView(ID3D11Device* device, const DXGI_FORMAT format, const bool multisamplingEnabled)
{
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	ZeroMemory(&shaderResourceViewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));

	shaderResourceViewDesc.Format = format;
	shaderResourceViewDesc.ViewDimension = multisamplingEnabled ? D3D11_SRV_DIMENSION_TEXTURE2DMS :  D3D11_SRV_DIMENSION_TEXTURE2D;
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
