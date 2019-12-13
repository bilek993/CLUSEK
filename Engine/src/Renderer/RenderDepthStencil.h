#pragma once
#include <d3d11.h>
#include <wrl/client.h>

class RenderDepthStencil final
{
public:
	bool Initialize(ID3D11Device* device, 
					int width, 
					int height, 
					int multisamplesCount, 
					int multisamplesQuality);

	ID3D11Texture2D* GetDepthStencilTextureBufferPointer() const;
	ID3D11DepthStencilView* GetDepthStencilViewPointer() const;
	ID3D11DepthStencilState* GetDepthStencilStatePointer() const;
private:
	bool InitializeDepthStencilTextureBuffer(	ID3D11Device* device, 
												int width, 
												int height, 
												int multisamplesCount,
												int multisamplesQuality);
	bool InitializeDepthStencilView(ID3D11Device* device);
	bool InitializeDepthStencilState(ID3D11Device* device);

	Microsoft::WRL::ComPtr<ID3D11Texture2D> DepthStencilTextureBuffer;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> DepthStencilView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> DepthStencilState;
};