#include "ReusablePostProcessing.h"
#include "../../Utils/StringUtil.h"

ReusablePostProcessing::ReusablePostProcessing(ID3D11DeviceContext* deviceContext, ID3D11Device* device,
	const int windowWidth, const int windowHeight, const DXGI_FORMAT renderTargetFormat, const std::string& name,
	const std::string& pixelShaderFilename) : BasePostProcessing(deviceContext, device)
{
	Logger::Debug("Preparing new post processing effect...");

	Name = name;

	if (!VertexShaderInstance.Initialize(Device, StringUtil::StringToWide("post_processing_vertex_shader.cso"), PositionVertex::Layout, PositionVertex::LayoutSize))
		Logger::Error("'post_processing_vertex_shader.cso' not initialized due to critical problem!");

	if (!PixelShaderInstance.Initialize(Device, StringUtil::StringToWide(pixelShaderFilename)))
		Logger::Error("'" + pixelShaderFilename + "' not initialized due to critical problem!");

	if (!OutputRenderTexture.Initialize(device, windowWidth, windowHeight, renderTargetFormat,
		1, 0))
		Logger::Error("OutputRenderTexture not initialized due to critical problem!");
}

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> ReusablePostProcessing::Process(
	ID3D11ShaderResourceView* const* inputResource) const
{
	UINT offset = 0;
	ChangeShader(VertexShaderInstance, PixelShaderInstance);

	DeviceContext->OMSetRenderTargets(1, OutputRenderTexture.GetAddressOfRenderTargetView(), nullptr);
	DeviceContext->PSSetShaderResources(0, 1, inputResource);

	Draw(VertexBufferInstance, IndexBufferInstance, offset, 1);

	return OutputRenderTexture.GetShaderResourceView();
}

std::string& ReusablePostProcessing::GetName()
{
	return Name;
}
