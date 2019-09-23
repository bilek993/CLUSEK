#pragma once
#include <wrl/client.h>
#include <WICTextureLoader.h>

class RenderTexture final
{
public:
	bool Initialize(ID3D11Device* device, int width, int height, DXGI_FORMAT renderTargetFormat,
		int multisamplesCount, int multisamplesQuality);

	ID3D11RenderTargetView* const* GetAddressOfRenderTargetView() const;
	ID3D11RenderTargetView* GetRenderTargetViewPointer() const;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetShaderResourceView() const;
private:
	bool InitializeRenderTexture2D(ID3D11Device* device, int width, int height, DXGI_FORMAT renderTargetFormat, 
		int multisamplesCount, int multisamplesQuality);
	bool InitializeRenderTargetView(ID3D11Device* device, DXGI_FORMAT renderTargetFormat,
		const bool multisampling);
	bool InitializeShaderResourceView(ID3D11Device* device, DXGI_FORMAT renderTargetFormat,
		bool multisampling);

	Microsoft::WRL::ComPtr<ID3D11Texture2D> RenderTexture2D;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> RenderTargetView;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> ShaderResourceView;
};
