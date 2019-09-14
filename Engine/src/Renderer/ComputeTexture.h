#pragma once
#include "Shaders/ComputeShader.h"

struct ComputeTexture final
{
	Microsoft::WRL::ComPtr<ID3D11Texture2D> Texture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> ShaderResourceView;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> UnorderedAccessView;

	int Width;
	int Height;
	int Levels;
};
