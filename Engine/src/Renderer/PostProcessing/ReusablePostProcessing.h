#pragma once
#include "BasePostProcessing.h"
#include "../RenderTexture.h"

class ReusablePostProcessing final : public BasePostProcessing
{
public:
	explicit ReusablePostProcessing(ID3D11DeviceContext* deviceContext, ID3D11Device* device, int windowWidth, int windowHeight, 
		DXGI_FORMAT renderTargetFormat, const std::string& name, const std::string& pixelShaderFilename);

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> Process(ID3D11ShaderResourceView* const* inputResource) const;
	std::string& GetName();
private:
	std::string Name;

	VertexShader VertexShaderInstance;
	PixelShader PixelShaderInstance;

	RenderTexture OutputRenderTexture;
};
