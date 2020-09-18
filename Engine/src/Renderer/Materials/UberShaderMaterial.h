#pragma once
#include <wrl/client.h>

struct UberShaderMaterial final
{
	float Alpha = 1.0f;
	float ThresholdAlpha = 0.0f;
	bool TwoSided = false;
	float HightWindSpeed = 0.001f;
	float HightWindScale = 0.001f;
	float HightWindBase = 1.0f;
	float LocalWindSpeed = 0.001f;
	float LocalWindScale = 0.001f;
	bool HightWindEnabled = false;
	bool LocalWindEnabled = false;
	std::shared_ptr<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> AlbedoTexture;
	std::shared_ptr<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> NormalTexture;
	std::shared_ptr<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> MetalicSmoothnessTexture;
	std::shared_ptr<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> OcclusionTexture;
	std::shared_ptr<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> EmissionTexture;
};
