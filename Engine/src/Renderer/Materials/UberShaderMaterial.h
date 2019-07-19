#pragma once
#include <wrl/client.h>

struct UberShaderMaterial final
{
	std::shared_ptr<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> MainTexture;
};
