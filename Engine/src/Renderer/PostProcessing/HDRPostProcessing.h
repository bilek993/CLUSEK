#pragma once
#include <d3d11.h>
#include "BasePostProcessing.h"
#include "../Shaders/PixelShader.h"
#include "../Shaders/VertexShader.h"

class HDRPostProcessing final : public BasePostProcessing
{
public:
	explicit HDRPostProcessing(ID3D11DeviceContext* deviceContext, ID3D11Device* device, int windowWidth,
		int windowHeight, DXGI_FORMAT renderTargetFormat);

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> Process(ID3D11ShaderResourceView* const* inputResource) const;
private:
	VertexShader HDRVertexShader;
	PixelShader HDRPixelShader;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> OutputRenderTexture;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> OutputRenderTargetView;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> OutputShaderResourceView;
};
