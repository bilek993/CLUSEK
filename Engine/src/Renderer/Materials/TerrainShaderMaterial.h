#pragma once
#include <wrl/client.h>
#include <memory>

struct TerrainShaderMaterial final
{
	std::shared_ptr<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> Heightmap;
	std::shared_ptr<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> Splatmap;

	float BaseTextureScale;
	float RedTextureScale;
	float GreenTextureScale;
	float BlueTextureScale;

	std::shared_ptr<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> BaseAlbedo;
	std::shared_ptr<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> RedAlbedo;
	std::shared_ptr<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> GreenAlbedo;
	std::shared_ptr<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> BlueAlbedo;
};
