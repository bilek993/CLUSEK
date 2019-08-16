#pragma once
#include <wrl/client.h>

struct UberShaderMaterial final
{
	float Alpha = 1.0f;
	std::shared_ptr<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> AlbedoTexture;
};
