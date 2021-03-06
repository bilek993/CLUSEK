#pragma once
#include <d3d11.h>
#include "BasePostProcessing.h"
#include "../Shaders/PixelShader.h"
#include "../Shaders/VertexShader.h"

class CopyToBackBufferPostProcessing final : public BasePostProcessing
{
public:
	explicit CopyToBackBufferPostProcessing(ID3D11DeviceContext* deviceContext, ID3D11Device* device,
		ID3D11RenderTargetView *const *backBufferRenderTargetViews);

	void Process(ID3D11ShaderResourceView* const* inputResource) const;
private:
	VertexShader CopyVertexShader;
	PixelShader CopyPixelShader;

	ID3D11RenderTargetView *const *BackBufferRenderTargetViews;
};
