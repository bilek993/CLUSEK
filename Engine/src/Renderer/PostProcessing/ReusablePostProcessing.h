#pragma once
#include "BasePostProcessing.h"
#include "../RenderTexture.h"

class ReusablePostProcessing final : public BasePostProcessing
{
public:
	explicit ReusablePostProcessing(ID3D11DeviceContext* deviceContext, ID3D11Device* device, int windowWidth,
		int windowHeight, DXGI_FORMAT renderTargetFormat, const std::string& pixelShaderFilename, const std::string& vertexShaderFilename);

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> Process(ID3D11ShaderResourceView* const* inputResource) const;
private:
	VertexShader VertexShaderInstance;
	PixelShader PixelShaderInstance;

	RenderTexture OutputRenderTexture;
};
