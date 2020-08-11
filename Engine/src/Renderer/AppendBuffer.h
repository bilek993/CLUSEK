#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include "../Utils/Logger.h"
#include "../Utils/BufferCalculationUtil.h"

template<class T>
class AppendBuffer final
{
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> Buffer;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> UnorderedAccessView;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> ShaderResourceView;
	UINT Stride = sizeof(T);
	UINT ItemCount = 0;

public:
	AppendBuffer() {}

	ID3D11Buffer* GetBuffer() const
	{
		return Buffer.Get();
	}

	ID3D11Buffer* const* GetAddressOfBuffer() const
	{
		return Buffer.GetAddressOf();
	}

	ID3D11UnorderedAccessView* GetUnorderedAccessView() const
	{
		return UnorderedAccessView.Get();
	}

	ID3D11UnorderedAccessView* const* GetAddressOfUnorderedAccessView() const
	{
		return UnorderedAccessView.GetAddressOf();
	}

	ID3D11ShaderResourceView* const* GetAddressOfShaderResourceView() const
	{
		return ShaderResourceView.GetAddressOf();
	}

	HRESULT Initialize(ID3D11Device *device, const UINT itemCount)
	{
		// Preparation to initialization

		Logger::Debug("Append buffer initialization...");

		if (Buffer.Get() != nullptr)
		{
			Buffer.Reset();
			Logger::Debug("Cleaning up memory for append buffer finished with success.");
		}

		ItemCount = itemCount;

		// Append Buffer initialization

		D3D11_BUFFER_DESC bufferDesc;
		ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));

		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		bufferDesc.StructureByteStride = BufferCalculationUtil::CalculateByteWidthWithPadding(sizeof(T));
		bufferDesc.ByteWidth = bufferDesc.StructureByteStride * ItemCount;
		bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
		bufferDesc.CPUAccessFlags = 0;

		auto hr = device->CreateBuffer(&bufferDesc, nullptr, Buffer.GetAddressOf());
		if (FAILED(hr))
			return hr;

		// Unordered Access View initialization

		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
		ZeroMemory(&uavDesc, sizeof(uavDesc));

		uavDesc.Format = DXGI_FORMAT_UNKNOWN;
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		uavDesc.Buffer.FirstElement = 0;
		uavDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_APPEND;
		uavDesc.Buffer.NumElements = ItemCount;

		hr = device->CreateUnorderedAccessView(Buffer.Get(), &uavDesc, UnorderedAccessView.GetAddressOf());
		if (FAILED(hr))
			return hr;

		// Shader Resource View initialization

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		ZeroMemory(&srvDesc, sizeof(srvDesc));

		srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		srvDesc.Buffer.FirstElement = 0;
		srvDesc.Buffer.NumElements = ItemCount;

		hr = device->CreateShaderResourceView(Buffer.Get(), &srvDesc, ShaderResourceView.GetAddressOf());
		if (FAILED(hr))
			return hr;

		// Finalization

		Logger::Debug("Append buffer initialization finished!");
		return hr;
	}
};