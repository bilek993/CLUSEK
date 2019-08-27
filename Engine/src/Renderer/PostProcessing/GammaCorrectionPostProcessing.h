#pragma once
#include <d3d11.h>
#include "BasePostProcessing.h"
#include "../Shaders/PixelShader.h"
#include "../Shaders/VertexShader.h"
#include "../RenderTexture.h"

class GammaCorrectionPostProcessing final : public BasePostProcessing
{
public:
	explicit GammaCorrectionPostProcessing(ID3D11DeviceContext* deviceContext, ID3D11Device* device, int windowWidth, 
		int windowHeight, DXGI_FORMAT renderTargetFormat);

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> Process(ID3D11ShaderResourceView* const* inputResource) const;
private:
	VertexShader GammaCorrectionVertexShader;
	PixelShader GammaCorrectionPixelShader;

	RenderTexture OutputRenderTexture;
};
