#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include <d3dcompiler.h>
#include <string>

class DomainShader final
{
public:
	bool Initialize(ID3D11Device* device, const std::wstring& shaderPath);
	ID3D11DomainShader *GetShader() const;
	ID3D10Blob *GetBuffer() const;
private:
	Microsoft::WRL::ComPtr<ID3D11DomainShader> Shader;
	Microsoft::WRL::ComPtr<ID3D10Blob> ShaderBuffer;
};
