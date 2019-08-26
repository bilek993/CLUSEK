#include "ToneMapperPostProcessing.h"

ToneMapperPostProcessing::ToneMapperPostProcessing(ID3D11DeviceContext* deviceContext, ID3D11Device* device, const int windowWidth,
	const int windowHeight, const DXGI_FORMAT renderTargetFormat) : BasePostProcessing(deviceContext, device)
{
	Logger::Debug("Preparing 'ToneMapper' post processing effect...");

	if (!ToneMapperVertexShader.Initialize(Device, L"tone_mapper_vertex_shader.cso", PositionVertex::Layout, PositionVertex::LayoutSize))
		Logger::Error("ToneMapperVertexShader not initialized due to critical problem!");

	if (!ToneMapperPixelShader.Initialize(Device, L"tone_mapper_pixel_shader.cso"))
		Logger::Error("ToneMapperPixelShader not initialized due to critical problem!");

	InitializeOutputTextureMap(windowWidth, windowHeight, OutputRenderTexture.GetAddressOf());
	InitializeOutputRenderTargetView(renderTargetFormat, OutputRenderTexture.Get(), OutputRenderTargetView.GetAddressOf());
	InitializeShaderResourceView(renderTargetFormat, OutputRenderTexture.Get(), OutputShaderResourceView.GetAddressOf());
}

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> ToneMapperPostProcessing::Process(
	ID3D11ShaderResourceView* const* inputResource) const
{
	UINT offset = 0;
	ChangeShader(ToneMapperVertexShader, ToneMapperPixelShader);

	DeviceContext->OMSetRenderTargets(1, OutputRenderTargetView.GetAddressOf(), nullptr);
	DeviceContext->PSSetShaderResources(0, 1, inputResource);

	Draw(VertexBufferInstance, IndexBufferInstance, offset, 1);

	return OutputShaderResourceView;
}
