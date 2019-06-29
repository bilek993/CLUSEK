#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include <d3dcompiler.h>
#include <string>

class VertexShader final
{
public:
	bool Initialize(Microsoft::WRL::ComPtr<ID3D11Device> &device, const std::wstring& shaderPath);
	ID3D11VertexShader *GetShader() const;
	ID3D10Blob *GetBuffer() const;
private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> Shader;
	Microsoft::WRL::ComPtr<ID3D10Blob> ShaderBuffer;
};
