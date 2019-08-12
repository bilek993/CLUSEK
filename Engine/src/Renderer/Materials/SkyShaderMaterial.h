#pragma once
#include <wrl/client.h>

struct SkyShaderMaterial final
{
	std::shared_ptr<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> SkyMap;
};
