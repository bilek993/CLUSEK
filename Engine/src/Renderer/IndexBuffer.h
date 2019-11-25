#pragma once
#include <wrl/client.h>
#include "VertexBuffer.h"


class IndexBuffer final
{
public:
	IndexBuffer() {}
	ID3D11Buffer* Get() const;
	ID3D11Buffer* const* GetAddressOf();
	UINT GetIndexCount() const;
	HRESULT Initialize(ID3D11Device *device, const DWORD * data, UINT indexCount);

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> Buffer;
	UINT IndexCount = 0;
};
