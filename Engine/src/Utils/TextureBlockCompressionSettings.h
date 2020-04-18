#pragma once
#include <string>

struct TextureBlockCompressionSettings final
{
	bool WIC_FLAGS_NONE = true;
	bool WIC_FLAGS_FORCE_RGB = false;
	bool WIC_FLAGS_NO_X2_BIAS = false;
	bool WIC_FLAGS_NO_16BPP = false;
	bool WIC_FLAGS_ALLOW_MONO = false;
	bool WIC_FLAGS_ALL_FRAMES = false;
	bool WIC_FLAGS_IGNORE_SRGB = false;
	bool WIC_FLAGS_FORCE_SRGB = false;
	bool WIC_FLAGS_FORCE_LINEAR = false;
	bool WIC_FLAGS_DITHER = false;
	bool WIC_FLAGS_DITHER_DIFFUSION = false;
	bool WIC_FLAGS_FILTER_POINT = false;
	bool WIC_FLAGS_FILTER_LINEAR = false;
	bool WIC_FLAGS_FILTER_CUBIC = false;
	bool WIC_FLAGS_FILTER_FANT = false;

	bool TEX_FILTER_DEFAULT = true;
	bool TEX_FILTER_POINT = false;
	bool TEX_FILTER_LINEAR = false;
	bool TEX_FILTER_CUBIC = false;
	bool TEX_FILTER_FANT = false;
	bool TEX_FILTER_BOX = false;
	bool TEX_FILTER_TRIANGLE = false;
	bool TEX_FILTER_DITHER = false;
	bool TEX_FILTER_DITHER_DIFFUSION = false;
	bool TEX_FILTER_WRAP_U = false;
	bool TEX_FILTER_WRAP_V = false;
	bool TEX_FILTER_WRAP_W = false;
	bool TEX_FILTER_MIRROR_U = false;
	bool TEX_FILTER_MIRROR_V = false;
	bool TEX_FILTER_MIRROR_W = false;
	bool TEX_FILTER_SRGB_IN = false;
	bool TEX_FILTER_SRGB_OUT = false;

	std::string CompressAlgorithm = "";
	float Threshold = 0.5f;
	bool TEX_COMPRESS_DEFAULT = true;
	bool TEX_COMPRESS_RGB_DITHER = false;
	bool TEX_COMPRESS_A_DITHER = false;
	bool TEX_COMPRESS_DITHER = false;
	bool TEX_COMPRESS_UNIFORM = false;
	bool TEX_COMPRESS_PARALLEL = false;
	bool TEX_COMPRESS_SRGB_IN = false;
	bool TEX_COMPRESS_SRGB_OUT = false;
	bool TEX_COMPRESS_BC7_USE_3SUBSETS = false;
	bool TEX_COMPRESS_BC7_QUICK = false;

	bool DDS_FLAGS_NONE = true;
	bool DDS_FLAGS_LEGACY_DWORD = false;
	bool DDS_FLAGS_BAD_DXTN_TAILS = false;
	bool DDS_FLAGS_NO_LEGACY_EXPANSION = false;
	bool DDS_FLAGS_NO_R10B10G10A2_FIXUP = false;
	bool DDS_FLAGS_FORCE_RGB = false;
	bool DDS_FLAGS_NO_16BPP = false;
	bool DDS_FLAGS_EXPAND_LUMINANCE = false;
	bool DDS_FLAGS_FORCE_DX10_EXT = false;
	bool DDS_FLAGS_FORCE_DX10_EXT_MISC2 = false;
};
