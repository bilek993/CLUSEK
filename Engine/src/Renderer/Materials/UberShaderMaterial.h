#pragma once
#include <wrl/client.h>

struct UberShaderMaterial final
{
	float Alpha = 1.0f;
	float ThresholdAlpha = 0.0f;
	bool TwoSided = false;
	float HightWindSpeed;
	float HightWindScale;
	float HightWindBase;
	float LocalWindSpeed;
	float LocalWindScale;
	bool HightWindEnabled;
	bool LocalWindEnabled;
	std::shared_ptr<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> AlbedoTexture;
	std::shared_ptr<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> NormalTexture;
	std::shared_ptr<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> MetalicSmoothnessTexture;
	std::shared_ptr<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> OcclusionTexture;
	std::shared_ptr<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> EmissionTexture;
};
