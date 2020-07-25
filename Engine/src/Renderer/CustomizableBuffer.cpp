#include "CustomizableBuffer.h"

#include "../Utils/Logger.h"

void CustomizableBuffer::HandleResetIfNeeded()
{
	if (Buffer.Get() != nullptr)
	{
		Buffer.Reset();
		Logger::Debug("Cleaning up memory for customizable buffer finished with success.");
	}
}

HRESULT CustomizableBuffer::InitializeBuffer(ID3D11Device* device, D3D11_BUFFER_DESC bufferDesc)
{
	return device->CreateBuffer(&bufferDesc, nullptr, Buffer.GetAddressOf());
}

HRESULT CustomizableBuffer::InitializeUnorderedAccessView(ID3D11Device* device, D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc)
{
	return device->CreateUnorderedAccessView(Buffer.Get(), &uavDesc, UnorderedAccessView.GetAddressOf());
}

HRESULT CustomizableBuffer::InitializeShaderResourceView(ID3D11Device* device, D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc)
{
	return device->CreateShaderResourceView(Buffer.Get(), &srvDesc, ShaderResourceView.GetAddressOf());
}

CustomizableBuffer::CustomizableBuffer()
{
	Logger::Debug("Created uninitialized Customizable Buffer!");
}

ID3D11Buffer* CustomizableBuffer::GetBuffer() const
{
	return Buffer.Get();
}

ID3D11Buffer* const* CustomizableBuffer::GetAddressOfBuffer() const
{
	return Buffer.GetAddressOf();
}

ID3D11UnorderedAccessView* CustomizableBuffer::GetUnorderedAccessView() const
{
	return UnorderedAccessView.Get();
}

ID3D11UnorderedAccessView* const* CustomizableBuffer::GetAddressOfUnorderedAccessView() const
{
	return UnorderedAccessView.GetAddressOf();
}

ID3D11ShaderResourceView* CustomizableBuffer::GetShaderResourceView() const
{
	return ShaderResourceView.Get();
}

ID3D11ShaderResourceView* const* CustomizableBuffer::GetAddressOfShaderResourceView() const
{
	return ShaderResourceView.GetAddressOf();
}

HRESULT CustomizableBuffer::Initialize(ID3D11Device* device, const D3D11_BUFFER_DESC bufferDesc)
{
	Logger::Debug("Customizable buffer initialization...");
	Logger::Debug("Buffer mode: ONLY BUFFER");
	
	const auto hr = InitializeBuffer(device, bufferDesc);
	if (FAILED(hr))
		return hr;

	Logger::Debug("Customizable buffer initialization finished!");
	return hr;
}

HRESULT CustomizableBuffer::Initialize(ID3D11Device* device, const D3D11_BUFFER_DESC bufferDesc, const D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc)
{
	Logger::Debug("Customizable buffer initialization...");
	Logger::Debug("Buffer mode: BUFFER & UAV");
	
	auto hr = InitializeBuffer(device, bufferDesc);
	if (FAILED(hr))
		return hr;

	hr = InitializeUnorderedAccessView(device, uavDesc);
	if (FAILED(hr))
		return hr;

	Logger::Debug("Customizable buffer initialization finished!");
	return hr;
}

HRESULT CustomizableBuffer::Initialize(ID3D11Device* device, const D3D11_BUFFER_DESC bufferDesc, const D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc)
{
	Logger::Debug("Customizable buffer initialization...");
	Logger::Debug("Buffer mode: BUFFER & SRV");
	
	auto hr = InitializeBuffer(device, bufferDesc);
	if (FAILED(hr))
		return hr;

	hr = InitializeShaderResourceView(device, srvDesc);
	if (FAILED(hr))
		return hr;

	Logger::Debug("Customizable buffer initialization finished!");
	return hr;
}

HRESULT CustomizableBuffer::Initialize(ID3D11Device* device, const D3D11_BUFFER_DESC bufferDesc, const D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc, const D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc)
{
	Logger::Debug("Customizable buffer initialization...");
	Logger::Debug("Buffer mode: BUFFER & UAV & SRV");
	
	auto hr = InitializeBuffer(device, bufferDesc);
	if (FAILED(hr))
		return hr;

	hr = InitializeUnorderedAccessView(device, uavDesc);
	if (FAILED(hr))
		return hr;

	hr = InitializeShaderResourceView(device, srvDesc);
	if (FAILED(hr))
		return hr;

	Logger::Debug("Customizable buffer initialization finished!");
	return hr;
}
