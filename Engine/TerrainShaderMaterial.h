#pragma once
#include <wrl/client.h>
#include <memory>
#include "src/Renderer/Shaders/DomainShader.h"

struct TerrainShaderMaterial final
{
	std::shared_ptr<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> Heightmap;
};
