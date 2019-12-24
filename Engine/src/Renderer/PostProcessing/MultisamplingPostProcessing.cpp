#include "MultisamplingPostProcessing.h"

MultisamplingPostProcessing::MultisamplingPostProcessing(ID3D11DeviceContext* deviceContext, ID3D11Device* device,
	const int windowWidth, const int windowHeight, const DXGI_FORMAT renderTargetFormat) : BasePostProcessing(deviceContext, device)
{
	Logger::Debug("Preparing 'Multisampling' post processing effect...");

	if (!MultisamplingVertexShader.Initialize(Device, L"post_processing_vertex_shader.cso", PositionVertex::Layout, PositionVertex::LayoutSize))
		Logger::Error("MultisamplingVertexShader not initialized due to critical problem!");

	if (!MultisamplingPixelShader.Initialize(Device, L"multisampling_pixel_shader.cso"))
		Logger::Error("MultisamplingPixelShader not initialized due to critical problem!");

	if (!OutputRenderTexture.Initialize(device, windowWidth, windowHeight, renderTargetFormat,
		1, 0))
		Logger::Error("OutputRenderTexture not initialized due to critical problem!");
}

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> MultisamplingPostProcessing::Process(
	ID3D11ShaderResourceView* const* inputResource) const
{
	UINT offset = 0;
	ChangeShader(MultisamplingVertexShader, MultisamplingPixelShader);

	DeviceContext->OMSetRenderTargets(1, OutputRenderTexture.GetAddressOfRenderTargetView(), nullptr);
	DeviceContext->PSSetShaderResources(0, 1, inputResource);

	Draw(VertexBufferInstance, IndexBufferInstance, offset, 1);

	return OutputRenderTexture.GetShaderResourceView();
}
