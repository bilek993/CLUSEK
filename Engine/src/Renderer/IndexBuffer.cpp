#include "IndexBuffer.h"
#include "../Utils/Logger.h"

ID3D11Buffer* IndexBuffer::Get() const
{
	return Buffer.Get();
}

ID3D11Buffer* const* IndexBuffer::GetAddressOf()
{
	return Buffer.GetAddressOf();
}

UINT IndexBuffer::GetIndexCount() const
{
	return IndexCount;
}

HRESULT IndexBuffer::Initialize(ID3D11Device *device, const DWORD *data, const UINT indexCount)
{
	Logger::Debug("Index buffer initialization...");

	if (Buffer.Get() != nullptr)
	{
		Buffer.Reset();
		Logger::Debug("Cleaning up memory for index buffer finished with success.");
	}

	IndexCount = indexCount;

	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory(&indexBufferDesc, sizeof(D3D11_BUFFER_DESC));

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(DWORD) * IndexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA indexBufferData;
	indexBufferData.pSysMem = data;
	return device->CreateBuffer(&indexBufferDesc, &indexBufferData, Buffer.GetAddressOf());;
}