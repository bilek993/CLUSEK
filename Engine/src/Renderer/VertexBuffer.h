#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include "../Utils/Logger.h"

template<class T>
class VertexBuffer final
{
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> Buffer;
	UINT Stride = sizeof(T);
	UINT VertexCount = 0;

public:
	VertexBuffer() {}

	VertexBuffer(const VertexBuffer<T>& ref)
	{
		Buffer = ref.Buffer;
		Stride = ref.Stride;
		VertexCount = ref.VertexCount;
	}

	VertexBuffer<T>& operator=(const VertexBuffer<T>& ref)
	{
		Buffer = ref.Buffer;
		Stride = ref.Stride;
		VertexCount = ref.VertexCount;
		return *this;
	}

	ID3D11Buffer* Get() const
	{
		return Buffer.Get();
	}

	ID3D11Buffer* const* GetAddressOf() const
	{
		return Buffer.GetAddressOf();
	}

	UINT GetVertexCount() const
	{
		return VertexCount;
	}

	UINT GetStride() const
	{
		return Stride;
	}

	const UINT *StridePtr() const
	{
		return &Stride;
	}

	HRESULT Initialize(ID3D11Device *device, T *data, UINT vertexCount)
	{
		Logger::Debug("Vertex buffer initialization...");

		if (Buffer.Get() != nullptr)
		{
			Buffer.Reset();
			Logger::Debug("Cleaning up memory for vertex buffer finished with success.");
		}

		VertexCount = vertexCount;

		D3D11_BUFFER_DESC vertexBufferDesc;
		ZeroMemory(&vertexBufferDesc, sizeof(D3D11_BUFFER_DESC));

		vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDesc.ByteWidth = sizeof(T) * VertexCount;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = 0;
		vertexBufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA vertexBufferData;
		ZeroMemory(&vertexBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
		vertexBufferData.pSysMem = data;

		return device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, Buffer.GetAddressOf());
	}
};
