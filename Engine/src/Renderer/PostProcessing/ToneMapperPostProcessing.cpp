#include "ToneMapperPostProcessing.h"

ToneMapperPostProcessing::ToneMapperPostProcessing(ID3D11DeviceContext* deviceContext, ID3D11Device* device, const int windowWidth,
	const int windowHeight, const DXGI_FORMAT renderTargetFormat) : BasePostProcessing(deviceContext, device)
{
	Logger::Debug("Preparing 'ToneMapper' post processing effect...");

	if (!ToneMapperVertexShader.Initialize(Device, L"tone_mapper_vertex_shader.cso", PositionVertex::Layout, PositionVertex::LayoutSize))
		Logger::Error("ToneMapperVertexShader not initialized due to critical problem!");

	if (!ToneMapperPixelShader.Initialize(Device, L"tone_mapper_pixel_shader.cso"))
		Logger::Error("ToneMapperPixelShader not initialized due to critical problem!");

	if (!OutputRenderTexture.Initialize(device, windowWidth, windowHeight, DXGI_FORMAT_R32G32B32A32_FLOAT))
		Logger::Error("OutputRenderTexture not initialized due to critical problem!");
}

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> ToneMapperPostProcessing::Process(
	ID3D11ShaderResourceView* const* inputResource) const
{
	UINT offset = 0;
	ChangeShader(ToneMapperVertexShader, ToneMapperPixelShader);

	DeviceContext->OMSetRenderTargets(1, OutputRenderTexture.GetAddressOfRenderTargetView(), nullptr);
	DeviceContext->PSSetShaderResources(0, 1, inputResource);

	Draw(VertexBufferInstance, IndexBufferInstance, offset, 1);

	return OutputRenderTexture.GetShaderResourceView();
}
