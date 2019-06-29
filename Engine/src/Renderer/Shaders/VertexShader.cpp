#include "VertexShader.h"
#include "../../Utils/Logger.h"

bool VertexShader::Initialize(Microsoft::WRL::ComPtr<ID3D11Device>& device, const std::wstring& shaderPath)
{
	Logger::Debug("Preparing to initialize vertex shader...");

	auto hr = D3DReadFileToBlob(shaderPath.c_str(), ShaderBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		Logger::Error("Failed to load shader file!");
		return false;
	}

	hr = device->CreateVertexShader(ShaderBuffer->GetBufferPointer(), ShaderBuffer->GetBufferSize(), nullptr, Shader.GetAddressOf());
	if (FAILED(hr))
	{
		Logger::Error("Failed to create vertex shader!");
		return false;
	}

	Logger::Debug("Vertex shader initialized.");
	return true;
}

ID3D11VertexShader* VertexShader::GetShader() const
{
	return Shader.Get();
}

ID3D10Blob* VertexShader::GetBuffer() const
{
	return ShaderBuffer.Get();
}
