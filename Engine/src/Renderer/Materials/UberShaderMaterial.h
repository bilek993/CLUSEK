#pragma once
#include <wrl/client.h>

struct UberShaderMaterial final
{
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> MainTexture;
};
