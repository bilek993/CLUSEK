#pragma once
#include <DDSTextureLoader.h>
#include <memory>
#include <wrl/client.h>

struct GrassShaderMaterial final
{
	std::shared_ptr<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> AlbedoTexture;
	std::shared_ptr<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> NormalTexture;
};
