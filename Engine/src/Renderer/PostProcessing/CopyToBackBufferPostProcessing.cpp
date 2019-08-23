#include "CopyToBackBufferPostProcessing.h"

CopyToBackBufferPostProcessing::CopyToBackBufferPostProcessing(ID3D11DeviceContext* deviceContext, ID3D11Device* device,
	ID3D11RenderTargetView *const *backBufferRenderTargetViews, ID3D11DepthStencilView *depthStencilView)
{
	Logger::Debug("Preparing 'CopyToBackBuffer' post processing effect...");

	DeviceContext = deviceContext;
	Device = device;

	BackBufferRenderTargetViews = backBufferRenderTargetViews;
	DepthStencilView = depthStencilView;

	if (!CopyVertexShader.Initialize(Device, L"copy_vertex_shader.cso", PositionVertex::Layout, PositionVertex::LayoutSize))
		Logger::Error("CopyVertexShader not initialized due to critical problem!");

	if (!CopyPixelShader.Initialize(Device, L"copy_pixel_shader.cso"))
		Logger::Error("CopyPixelShader not initialized due to critical problem!");

	GenerateQuad();
}

void CopyToBackBufferPostProcessing::Process() const
{
	UINT offset = 0;
	ChangeShader(CopyVertexShader, CopyPixelShader);

	DeviceContext->OMSetRenderTargets(1, BackBufferRenderTargetViews, DepthStencilView);

	Draw(VertexBufferInstance, IndexBufferInstance, offset);
}
