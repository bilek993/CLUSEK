#include "VertexShader.h"
#include "../../Utils/Logger.h"

bool VertexShader::Initialize(Microsoft::WRL::ComPtr<ID3D11Device>& device, const std::wstring& shaderPath,
	D3D11_INPUT_ELEMENT_DESC *layoutDescription, const UINT numberOfElements)
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

	hr = device->CreateInputLayout(layoutDescription, numberOfElements, ShaderBuffer->GetBufferPointer(),
		ShaderBuffer->GetBufferSize(), InputLayout.GetAddressOf());
	if (FAILED(hr))
	{
		Logger::Error("Creating input layout failed!");
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

ID3D11InputLayout* VertexShader::GetInputLayout() const
{
	return InputLayout.Get();
}
