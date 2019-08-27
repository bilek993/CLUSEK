#pragma once
#include <wrl/client.h>
#include <WICTextureLoader.h>

class RenderTexture final
{
public:
	bool Initialize(ID3D11Device* device, int width, int height, DXGI_FORMAT renderTargetFormat);

	ID3D11RenderTargetView* const* GetAddressOfRenderTargetView() const;
	ID3D11RenderTargetView* GetRenderTargetViewPointer() const;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetShaderResourceView() const;
private:
	bool InitializeRenderTexture2D(ID3D11Device* device, int width, int height, DXGI_FORMAT renderTargetFormat);
	bool InitializeRenderTargetView(ID3D11Device* device, DXGI_FORMAT renderTargetFormat);
	bool InitializeShaderResourceView(ID3D11Device* device, DXGI_FORMAT renderTargetFormat);

	Microsoft::WRL::ComPtr<ID3D11Texture2D> RenderTexture2D;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> RenderTargetView;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> ShaderResourceView;
};
