#include "ShadowRemapperPostProcessing.h"

ShadowRemapperPostProcessing::ShadowRemapperPostProcessing(ID3D11DeviceContext* deviceContext, ID3D11Device* device,
	const int width, const int height, const DXGI_FORMAT renderTargetFormat,
	const float cameraNearZ, const float cameraFarZ, const float bias) : BasePostProcessing(deviceContext, device)
{
	Logger::Debug("Preparing 'ShadowRemapper' post processing effect...");

	if (!ShadowRemapperVertexShader.Initialize(Device, L"post_processing_vertex_shader.cso", PositionVertex::Layout, PositionVertex::LayoutSize))
		Logger::Error("ShadowRemapperVertexShader not initialized due to critical problem!");

	if (!ShadowRemapperPixelShader.Initialize(Device, L"shadow_remapper_pixel_shader.cso"))
		Logger::Error("ShadowRemapperPixelShader not initialized due to critical problem!");

	const auto hr = ShadowRemapperConstantBuffer.Initialize(device, deviceContext);
	if (FAILED(hr))
		Logger::Error("ShadowRemapperConstantBuffer not initialized due to critical problem!");

	SetValuesInConstantBuffer(cameraNearZ, cameraFarZ, bias);

	if (!OutputRenderTexture.Initialize(device, width, height, renderTargetFormat,
		1, 0))
		Logger::Error("OutputRenderTexture not initialized due to critical problem!");
}

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> ShadowRemapperPostProcessing::Process(
	ID3D11ShaderResourceView* const* inputResource) const
{
	UINT offset = 0;
	ChangeShader(ShadowRemapperVertexShader, ShadowRemapperPixelShader);

	DeviceContext->OMSetRenderTargets(1, OutputRenderTexture.GetAddressOfRenderTargetView(), nullptr);
	DeviceContext->PSSetShaderResources(0, 1, inputResource);

	DeviceContext->PSSetConstantBuffers(0, 1, ShadowRemapperConstantBuffer.GetAddressOf());

	Draw(VertexBufferInstance, IndexBufferInstance, offset, 1);

	return OutputRenderTexture.GetShaderResourceView();
}

void ShadowRemapperPostProcessing::SetValuesInConstantBuffer(const float cameraNearZ, const float cameraFarZ, const float bias)
{
	ShadowRemapperConstantBuffer.Data.NearZ = cameraNearZ;
	ShadowRemapperConstantBuffer.Data.FarZ = cameraFarZ;
	ShadowRemapperConstantBuffer.Data.Bias = bias;
	ShadowRemapperConstantBuffer.ApplyChanges();
}
