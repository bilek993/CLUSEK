#include "DomainShader.h"
#include "../../Utils/Logger.h"

bool DomainShader::Initialize(ID3D11Device* device, const std::wstring& shaderPath)
{
	Logger::Debug("Preparing to initialize domain shader...");

	auto hr = D3DReadFileToBlob(shaderPath.c_str(), ShaderBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		Logger::Error("Failed to load shader file!");
		return false;
	}

	hr = device->CreateDomainShader(ShaderBuffer->GetBufferPointer(), ShaderBuffer->GetBufferSize(), nullptr, Shader.GetAddressOf());
	if (FAILED(hr))
	{
		Logger::Error("Failed to create domain shader!");
		return false;
	}

	Logger::Debug("Domain shader initialized.");
	return true;
}

ID3D11DomainShader* DomainShader::GetShader() const
{
	return Shader.Get();
}

ID3D10Blob* DomainShader::GetBuffer() const
{
	return ShaderBuffer.Get();
}
