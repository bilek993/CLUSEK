#include "BasePostProcessing.h"
#include "../Shaders/VertexShader.h"
#include "../Shaders/PixelShader.h"
#include "../Generators/QuadGenerator.h"

BasePostProcessing::BasePostProcessing(ID3D11DeviceContext* deviceContext, ID3D11Device* device) : DeviceContext(deviceContext), Device(device)
{
	QuadGenerator::Generate(Device, VertexBufferInstance, IndexBufferInstance, -1.0f, -1.0f, 1.0f, 1.0f);
}

void BasePostProcessing::ChangeShader(const VertexShader& vertexShader, const PixelShader& pixelShader) const
{
	DeviceContext->IASetInputLayout(vertexShader.GetInputLayout());
	DeviceContext->VSSetShader(vertexShader.GetShader(), nullptr, 0);
	DeviceContext->PSSetShader(pixelShader.GetShader(), nullptr, 0);
}