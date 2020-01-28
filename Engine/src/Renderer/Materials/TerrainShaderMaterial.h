#pragma once
#include <wrl/client.h>
#include <memory>

struct TerrainShaderMaterial final
{
	std::shared_ptr<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> Heightmap;
};
