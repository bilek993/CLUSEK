#pragma once
#include "BasePostProcessing.h"
#include "../RenderTexture.h"
#include "../ConstantBuffer.h"
#include "../ConstantBufferTypes/ShadowRemapperBuffer.h"

class ShadowRemapperPostProcessing final : public BasePostProcessing
{
public:
	explicit ShadowRemapperPostProcessing(ID3D11DeviceContext* deviceContext, ID3D11Device* device, int width, int height,
		DXGI_FORMAT renderTargetFormat, float cameraNearZ, float cameraFarZ, float bias);

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> Process(ID3D11ShaderResourceView* const* inputResource) const;

private:
	void SetValuesInConstantBuffer(float cameraNearZ, float cameraFarZ, float bias);

	VertexShader ShadowRemapperVertexShader;
	PixelShader ShadowRemapperPixelShader;

	ConstantBuffer<ShadowRemapperBuffer> ShadowRemapperConstantBuffer;

	RenderTexture OutputRenderTexture;
};
