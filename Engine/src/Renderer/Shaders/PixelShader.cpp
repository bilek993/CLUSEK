#include "PixelShader.h"
#include "../../Utils/Logger.h"

bool PixelShader::Initialize(ID3D11Device* device, const std::wstring& shaderPath)
{
	Logger::Debug("Preparing to initialize pixel shader...");

	auto hr = D3DReadFileToBlob(shaderPath.c_str(), ShaderBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		Logger::Error("Failed to load shader file!");
		return false;
	}

	hr = device->CreatePixelShader(ShaderBuffer->GetBufferPointer(), ShaderBuffer->GetBufferSize(), nullptr, Shader.GetAddressOf());
	if (FAILED(hr))
	{
		Logger::Error("Failed to create pixel shader!");
		return false;
	}

	Logger::Debug("Pixel shader initialized.");
	return true;
}

ID3D11PixelShader* PixelShader::GetShader() const
{
	return Shader.Get();
}

ID3D10Blob* PixelShader::GetBuffer() const
{
	return ShaderBuffer.Get();
}
