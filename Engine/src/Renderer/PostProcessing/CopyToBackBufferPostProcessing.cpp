#include "CopyToBackBufferPostProcessing.h"
#include "../Generators/QuadGenerator.h"

CopyToBackBufferPostProcessing::CopyToBackBufferPostProcessing(ID3D11DeviceContext* deviceContext, ID3D11Device* device,
	ID3D11RenderTargetView *const *backBufferRenderTargetViews, ID3D11DepthStencilView *depthStencilView)
{
	DeviceContext = deviceContext;
	Device = device;

	BackBufferRenderTargetViews = backBufferRenderTargetViews;
	DepthStencilView = depthStencilView;

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

	VertexBuffer<PositionVertex> vertexBuffer;
	IndexBuffer indexBuffer;
	QuadGenerator::Generate(Device, vertexBuffer, indexBuffer, -1.0f, -1.0f, 1.0f, 1.0f);

	Draw(vertexBuffer, indexBuffer, offset);
}
