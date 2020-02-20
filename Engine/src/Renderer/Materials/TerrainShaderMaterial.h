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

	std::shared_ptr<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> BaseNormalTexture;
	std::shared_ptr<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> RedNormalTexture;
	std::shared_ptr<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> GreenNormalTexture;
	std::shared_ptr<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> BlueNormalTexture;

	std::shared_ptr<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> BaseMetalicSmoothnessTexture;
	std::shared_ptr<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> RedMetalicSmoothnessTexture;
	std::shared_ptr<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> GreenMetalicSmoothnessTexture;
	std::shared_ptr<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> BlueMetalicSmoothnessTexture;

	std::shared_ptr<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> BaseOcclusionTexture;
	std::shared_ptr<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> RedOcclusionTexture;
	std::shared_ptr<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> GreenOcclusionTexture;
	std::shared_ptr<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> BlueOcclusionTexture;
};
