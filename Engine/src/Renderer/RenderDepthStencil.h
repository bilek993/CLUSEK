#pragma once
#include <d3d11.h>
#include <wrl/client.h>

class RenderDepthStencil final
{
public:
	bool Initialize(ID3D11Device* device,
					int width,
					int height,
					DXGI_FORMAT format,
					int multisamplesCount,
					int multisamplesQuality,
					bool useShaderResourceView,
					bool useDepthStencilState);

	ID3D11Texture2D* GetDepthStencilTextureBufferPointer() const;
	ID3D11DepthStencilView* GetDepthStencilViewPointer() const;
	ID3D11ShaderResourceView* const* GetAddressOfShaderResourceView() const;
	ID3D11DepthStencilState* GetDepthStencilStatePointer() const;
private:
	bool InitializeDepthStencilTextureBuffer(	ID3D11Device* device,
												int width,
												int height,
												DXGI_FORMAT format,
												int multisamplesCount,
												int multisamplesQuality,
												bool useShaderResourceView);
	bool InitializeDepthStencilView(ID3D11Device* device);
	bool InitializeShaderResourceView(ID3D11Device* device, DXGI_FORMAT format);
	bool InitializeDepthStencilState(ID3D11Device* device);

	Microsoft::WRL::ComPtr<ID3D11Texture2D> DepthStencilTextureBuffer;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> DepthStencilView;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> ShaderResourceView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> DepthStencilState;
};