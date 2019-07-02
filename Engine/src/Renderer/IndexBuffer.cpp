#include "IndexBuffer.h"

ID3D11Buffer* IndexBuffer::Get() const
{
	return Buffer.Get();
}

ID3D11Buffer* const* IndexBuffer::GetAddressOf()
{
	return Buffer.GetAddressOf();
}

UINT IndexBuffer::GetBufferSize() const
{
	return BufferSize;
}

HRESULT IndexBuffer::Initialize(ID3D11Device *device, const DWORD *data, const UINT numIndices)
{
	BufferSize = numIndices;

	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory(&indexBufferDesc, sizeof(D3D11_BUFFER_DESC));

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(DWORD)*numIndices;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA indexBufferData;
	indexBufferData.pSysMem = data;
	return device->CreateBuffer(&indexBufferDesc, &indexBufferData, Buffer.GetAddressOf());;
}