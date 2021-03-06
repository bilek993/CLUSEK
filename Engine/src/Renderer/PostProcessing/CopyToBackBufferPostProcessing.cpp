#include "CopyToBackBufferPostProcessing.h"

CopyToBackBufferPostProcessing::CopyToBackBufferPostProcessing(ID3D11DeviceContext* deviceContext, ID3D11Device* device,
	ID3D11RenderTargetView* const * backBufferRenderTargetViews) : BasePostProcessing(deviceContext, device)
{
	Logger::Debug("Preparing 'CopyToBackBuffer' post processing effect...");

	BackBufferRenderTargetViews = backBufferRenderTargetViews;

	if (!CopyVertexShader.Initialize(Device, L"post_processing_vertex_shader.cso", PositionVertex::Layout, PositionVertex::LayoutSize))
		Logger::Error("CopyVertexShader not initialized due to critical problem!");

	if (!CopyPixelShader.Initialize(Device, L"copy_pixel_shader.cso"))
		Logger::Error("CopyPixelShader not initialized due to critical problem!");
}

void CopyToBackBufferPostProcessing::Process(ID3D11ShaderResourceView* const* inputResource) const
{
	UINT offset = 0;
	ChangeShader(CopyVertexShader, CopyPixelShader);

	DeviceContext->OMSetRenderTargets(1, BackBufferRenderTargetViews, nullptr);
	DeviceContext->PSSetShaderResources(0, 1, inputResource);

	Draw(VertexBufferInstance, IndexBufferInstance, offset, 1);
}
