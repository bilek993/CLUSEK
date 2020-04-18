#pragma once
#include <string>
#include <DirectXTexP.h>

class TextureBlockCompression final
{
public:
	static void Compress(	bool* loadingState,
							const std::string* inputPath,
							const std::string* outputPath);

	inline static const char* const SUPPORTED_ALGORITHMS[] = {
		"BC1_TYPELESS",
		"BC1_UNORM",
		"BC1_UNORM_SRGB",
		"BC2_TYPELESS",
		"BC2_UNORM",
		"BC2_UNORM_SRGB",
		"BC3_TYPELESS",
		"BC3_UNORM",
		"BC3_UNORM_SRGB",
		"BC4_TYPELESS",
		"BC4_UNORM",
		"BC4_SNORM",
		"BC5_TYPELESS",
		"BC5_UNORM",
		"BC5_SNORM",
		"BC6H_TYPELESS",
		"BC6H_UF16",
		"BC6H_SF16",
		"BC7_TYPELESS",
		"BC7_UNORM",
		"BC7_UNORM_SRGB",
	};
};
