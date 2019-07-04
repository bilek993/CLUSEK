#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include "../Utils/Logger.h"
#include "ConstantBufferTypes.h"

template<class T>
class ConstantBuffer final
{
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> Buffer;
	ID3D11DeviceContext *DeviceContext = nullptr;
public:
	ConstantBuffer() {}

	T Data;

	ID3D11Buffer* Get() const
	{
		return Buffer.Get();
	}

	ID3D11Buffer* const* GetAddressOf() const
	{
		return Buffer.GetAddressOf();
	}

	HRESULT Initialize(ID3D11Device *device, ID3D11DeviceContext *deviceContext)
	{
		Logger::Debug("Constant buffer initialization...");

		DeviceContext = deviceContext;

		D3D11_BUFFER_DESC constBufferDesc;
		ZeroMemory(&constBufferDesc, sizeof(D3D11_BUFFER_DESC));

		constBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		constBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		constBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		constBufferDesc.MiscFlags = 0;
		// For buffer alignment data must be packed into 4-byte boundaries (more info: https://docs.microsoft.com/en-us/windows/win32/direct3dhlsl/dx-graphics-hlsl-packing-rules).
		constBufferDesc.ByteWidth = static_cast<UINT>(sizeof(CB_VS_UberVertexShader) + (16 - (sizeof(CB_VS_UberVertexShader) % 16)));
		constBufferDesc.StructureByteStride = 0;

		return device->CreateBuffer(&constBufferDesc, nullptr, Buffer.GetAddressOf());
	}

	bool ApplyChanges()
	{
		D3D11_MAPPED_SUBRESOURCE mappedSubresource;
		const auto hr = DeviceContext->Map(Buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
		if (FAILED(hr))
		{
			Logger::Warning("Failed to map resources.");
			return false;
		}

		CopyMemory(mappedSubresource.pData, &Data, sizeof(CB_VS_UberVertexShader));
		DeviceContext->Unmap(Buffer.Get(), 0);
		return true;
	}
};
