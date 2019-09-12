#include "ComputeShader.h"
#include "../../Utils/Logger.h"

bool ComputeShader::Initialize(ID3D11Device* device, const std::wstring& shaderPath)
{
	Logger::Debug("Preparing to initialize compute shader...");

	auto hr = D3DReadFileToBlob(shaderPath.c_str(), ShaderBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		Logger::Error("Failed to load shader file!");
		return false;
	}

	hr = device->CreateComputeShader(ShaderBuffer->GetBufferPointer(), ShaderBuffer->GetBufferSize(), nullptr, Shader.GetAddressOf());
	if (FAILED(hr))
	{
		Logger::Error("Failed to create compute shader!");
		return false;
	}

	Logger::Debug("Compute shader initialized.");
	return true;
}

ID3D11ComputeShader* ComputeShader::GetShader() const
{
	return Shader.Get();
}

ID3D10Blob* ComputeShader::GetBuffer() const
{
	return ShaderBuffer.Get();
}
