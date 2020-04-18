#pragma once
#include <string>
#include <DirectXTexP.h>
#include "TextureBlockCompressionSettings.h"

#define HANDLE_FLAG(FLAG_VAR, FLAG_VAL, SETTINGS) if (SETTINGS->FLAG_VAL) FLAG_VAR |= DirectX::FLAG_VAL
#define HANDLE_FORMAT(FORMAT, INPUT) if (INPUT == #FORMAT) return FORMAT;

class TextureBlockCompression final
{
public:
	static void Compress(	bool* loadingState,
							const std::string* inputPath,
							const std::string* outputPath,
							const TextureBlockCompressionSettings* compressionSettings);

	inline static const char* const SUPPORTED_ALGORITHMS[] = {
		"DXGI_FORMAT_BC1_TYPELESS",
		"DXGI_FORMAT_BC1_UNORM",
		"DXGI_FORMAT_BC1_UNORM_SRGB",
		"DXGI_FORMAT_BC2_TYPELESS",
		"DXGI_FORMAT_BC2_UNORM",
		"DXGI_FORMAT_BC2_UNORM_SRGB",
		"DXGI_FORMAT_BC3_TYPELESS",
		"DXGI_FORMAT_BC3_UNORM",
		"DXGI_FORMAT_BC3_UNORM_SRGB",
		"DXGI_FORMAT_BC4_TYPELESS",
		"DXGI_FORMAT_BC4_UNORM",
		"DXGI_FORMAT_BC4_SNORM",
		"DXGI_FORMAT_BC5_TYPELESS",
		"DXGI_FORMAT_BC5_UNORM",
		"DXGI_FORMAT_BC5_SNORM",
		"DXGI_FORMAT_BC6H_TYPELESS",
		"DXGI_FORMAT_BC6H_UF16",
		"DXGI_FORMAT_BC6H_SF16",
		"DXGI_FORMAT_BC7_TYPELESS",
		"DXGI_FORMAT_BC7_UNORM",
		"DXGI_FORMAT_BC7_UNORM_SRGB",
	};

private:
	static DWORD GenerateFlagsForInput(const TextureBlockCompressionSettings* compressionSettings);
	static DWORD GenerateFlagsForMipMaps(const TextureBlockCompressionSettings* compressionSettings);
	static DXGI_FORMAT GenerateCompressionFormat(const TextureBlockCompressionSettings* compressionSettings);
	static DWORD GenerateFlagsForCompression(const TextureBlockCompressionSettings* compressionSettings);
	static DWORD GenerateFlagsForOutput(const TextureBlockCompressionSettings* compressionSettings);
};
