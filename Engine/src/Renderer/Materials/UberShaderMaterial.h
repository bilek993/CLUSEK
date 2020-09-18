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
	bool HightWindEnabled;
	std::shared_ptr<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> AlbedoTexture;
	std::shared_ptr<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> NormalTexture;
	std::shared_ptr<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> MetalicSmoothnessTexture;
	std::shared_ptr<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> OcclusionTexture;
	std::shared_ptr<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> EmissionTexture;
};
