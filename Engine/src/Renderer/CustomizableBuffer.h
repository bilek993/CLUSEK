#pragma once
#include <d3d11.h>
#include <wrl/client.h>

class CustomizableBuffer final
{
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> Buffer;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> UnorderedAccessView;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> ShaderResourceView;

	void HandleResetIfNeeded();

	HRESULT InitializeBuffer(ID3D11Device *device, D3D11_BUFFER_DESC bufferDesc);
	HRESULT InitializeUnorderedAccessView(ID3D11Device *device, D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc);
	HRESULT InitializeShaderResourceView(ID3D11Device *device, D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc);

public:
	CustomizableBuffer();

	ID3D11Buffer* GetBuffer() const;
	ID3D11Buffer* const* GetAddressOfBuffer() const;

	ID3D11UnorderedAccessView* GetUnorderedAccessView() const;
	ID3D11UnorderedAccessView* const* GetAddressOfUnorderedAccessView() const;

	ID3D11ShaderResourceView* GetShaderResourceView() const;
	ID3D11ShaderResourceView* const* GetAddressOfShaderResourceView() const;

	HRESULT Initialize(ID3D11Device *device, D3D11_BUFFER_DESC bufferDesc);

	HRESULT Initialize(ID3D11Device *device, D3D11_BUFFER_DESC bufferDesc, D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc);

	HRESULT Initialize(ID3D11Device *device, D3D11_BUFFER_DESC bufferDesc, D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc);

	HRESULT Initialize(ID3D11Device *device, D3D11_BUFFER_DESC bufferDesc, D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc, D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc);
};
