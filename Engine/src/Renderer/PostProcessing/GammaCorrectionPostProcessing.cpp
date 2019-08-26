#include "GammaCorrectionPostProcessing.h"

GammaCorrectionPostProcessing::GammaCorrectionPostProcessing(ID3D11DeviceContext* deviceContext, 
	ID3D11Device* device, const int windowWidth, const int windowHeight, const DXGI_FORMAT renderTargetFormat) : BasePostProcessing(deviceContext, device)
{
	Logger::Debug("Preparing 'GammaCorrection' post processing effect...");

	if (!GammaCorrectionVertexShader.Initialize(Device, L"gamma_correction_vertex_shader.cso", PositionVertex::Layout, PositionVertex::LayoutSize))
		Logger::Error("GammaCorrectionVertexShader not initialized due to critical problem!");

	if (!GammaCorrectionPixelShader.Initialize(Device, L"gamma_correction_pixel_shader.cso"))
		Logger::Error("GammaCorrectionPixelShader not initialized due to critical problem!");

	InitializeOutputTextureMap(windowWidth, windowHeight, OutputRenderTexture.GetAddressOf());
	InitializeOutputRenderTargetView(renderTargetFormat, OutputRenderTexture.Get(), OutputRenderTargetView.GetAddressOf());
	InitializeShaderResourceView(renderTargetFormat, OutputRenderTexture.Get(), OutputShaderResourceView.GetAddressOf());
}

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GammaCorrectionPostProcessing::Process(ID3D11ShaderResourceView* const* inputResource) const
{
	UINT offset = 0;
	ChangeShader(GammaCorrectionVertexShader, GammaCorrectionPixelShader);

	DeviceContext->OMSetRenderTargets(1, OutputRenderTargetView.GetAddressOf(), nullptr);
	DeviceContext->PSSetShaderResources(0, 1, inputResource);

	Draw(VertexBufferInstance, IndexBufferInstance, offset, 1);

	return OutputShaderResourceView;
}
