#include "BasePostProcessing.h"
#include "../Shaders/VertexShader.h"
#include "../Shaders/PixelShader.h"

void BasePostProcessing::ChangeShader(const VertexShader& vertexShader, const PixelShader& pixelShader) const
{
	DeviceContext->IASetInputLayout(vertexShader.GetInputLayout());
	DeviceContext->VSSetShader(vertexShader.GetShader(), nullptr, 0);
	DeviceContext->PSSetShader(pixelShader.GetShader(), nullptr, 0);
}
