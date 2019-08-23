#include "CopyToBackBufferPostProcessing.h"

CopyToBackBufferPostProcessing::CopyToBackBufferPostProcessing(ID3D11DeviceContext* deviceContext, ID3D11Device* device,
	ID3D11RenderTargetView* const * backBufferRenderTargetViews, ID3D11DepthStencilView* depthStencilView, 
	ID3D11ShaderResourceView* const* inputResource) : BasePostProcessing(deviceContext, device)
{
	Logger::Debug("Preparing 'CopyToBackBuffer' post processing effect...");

	BackBufferRenderTargetViews = backBufferRenderTargetViews;
	DepthStencilView = depthStencilView;

	InputResource = inputResource;

	if (!CopyVertexShader.Initialize(Device, L"copy_vertex_shader.cso", PositionVertex::Layout, PositionVertex::LayoutSize))
		Logger::Error("CopyVertexShader not initialized due to critical problem!");

	if (!CopyPixelShader.Initialize(Device, L"copy_pixel_shader.cso"))
		Logger::Error("CopyPixelShader not initialized due to critical problem!");
}

void CopyToBackBufferPostProcessing::Process() const
{
	UINT offset = 0;
	ChangeShader(CopyVertexShader, CopyPixelShader);

	DeviceContext->OMSetRenderTargets(1, BackBufferRenderTargetViews, DepthStencilView);
	DeviceContext->PSSetShaderResources(0, 1, InputResource);

	Draw(VertexBufferInstance, IndexBufferInstance, offset, 1);
}
