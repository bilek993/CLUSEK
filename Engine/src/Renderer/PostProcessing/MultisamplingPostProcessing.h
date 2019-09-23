#pragma once
#include "BasePostProcessing.h"
#include "../RenderTexture.h"

class MultisamplingPostProcessing final : public BasePostProcessing
{
public:
	explicit MultisamplingPostProcessing(ID3D11DeviceContext* deviceContext, ID3D11Device* device, int windowWidth, int windowHeight,
		DXGI_FORMAT renderTargetFormat);

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> Process(ID3D11ShaderResourceView* const* inputResource) const;
private:
	VertexShader MultisamplingVertexShader;
	PixelShader MultisamplingPixelShader;

	RenderTexture OutputRenderTexture;
};
