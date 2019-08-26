#include "HDRPostProcessing.h"

HDRPostProcessing::HDRPostProcessing(ID3D11DeviceContext* deviceContext, ID3D11Device* device, const int windowWidth,
	const int windowHeight, const DXGI_FORMAT renderTargetFormat) : BasePostProcessing(deviceContext, device)
{
	Logger::Debug("Preparing 'HDR' post processing effect...");

	if (!HDRVertexShader.Initialize(Device, L"hdr_vertex_shader.cso", PositionVertex::Layout, PositionVertex::LayoutSize))
		Logger::Error("HDRVertexShader not initialized due to critical problem!");

	if (!HDRPixelShader.Initialize(Device, L"hdr_pixel_shader.cso"))
		Logger::Error("HDRPixelShader not initialized due to critical problem!");

	InitializeOutputTextureMap(windowWidth, windowHeight, OutputRenderTexture.GetAddressOf());
	InitializeOutputRenderTargetView(renderTargetFormat, OutputRenderTexture.Get(), OutputRenderTargetView.GetAddressOf());
	InitializeShaderResourceView(renderTargetFormat, OutputRenderTexture.Get(), OutputShaderResourceView.GetAddressOf());
}

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> HDRPostProcessing::Process(
	ID3D11ShaderResourceView* const* inputResource) const
{
	UINT offset = 0;
	ChangeShader(HDRVertexShader, HDRPixelShader);

	DeviceContext->OMSetRenderTargets(1, OutputRenderTargetView.GetAddressOf(), nullptr);
	DeviceContext->PSSetShaderResources(0, 1, inputResource);

	Draw(VertexBufferInstance, IndexBufferInstance, offset, 1);

	return OutputShaderResourceView;
}
