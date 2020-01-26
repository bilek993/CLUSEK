#include "HullShader.h"
#include "../../Utils/Logger.h"

bool HullShader::Initialize(ID3D11Device* device, const std::wstring& shaderPath)
{
	Logger::Debug("Preparing to initialize hull shader...");

	auto hr = D3DReadFileToBlob(shaderPath.c_str(), ShaderBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		Logger::Error("Failed to load shader file!");
		return false;
	}

	hr = device->CreateHullShader(ShaderBuffer->GetBufferPointer(), ShaderBuffer->GetBufferSize(), nullptr, Shader.GetAddressOf());
	if (FAILED(hr))
	{
		Logger::Error("Failed to create hull shader!");
		return false;
	}

	Logger::Debug("Hull shader initialized.");
	return true;
}

ID3D11HullShader* HullShader::GetShader() const
{
	return Shader.Get();
}

ID3D10Blob* HullShader::GetBuffer() const
{
	return ShaderBuffer.Get();
}
