#include "TextureBlockCompression.h"
#include "StringUtil.h"
#include "Logger.h"

void TextureBlockCompression::Compress(	bool* loadingState, 
										const std::string* inputPath, 
										const std::string* outputPath,
										const TextureBlockCompressionSettings* compressionSettings)
{
	// Prepare for compression

	*loadingState = true;
	Logger::Debug("Preparing compression...");

	DirectX::ScratchImage inputImage;
	DirectX::ScratchImage inputImageWithMipMaps;
	DirectX::ScratchImage outputImage;

	// Load input image

	Logger::Debug("Loading input image...");

	auto hr = LoadFromWICFile(	StringUtil::StringToWide(*inputPath).c_str(),
								GenerateFlagsForInput(compressionSettings),
								nullptr,
								inputImage);
	if (FAILED(hr))
	{
		Logger::Error("Couldn't load input file!");
		*loadingState = false;
		return;
	}

	// Generating mip-maps

	Logger::Debug("Generating mip-maps for image...");

	hr = GenerateMipMaps(	inputImage.GetImages(), 
							inputImage.GetImageCount(), 
							inputImage.GetMetadata(), 
							GenerateFlagsForMipMaps(compressionSettings),
							0, 
							inputImageWithMipMaps);
	if (FAILED(hr))
	{
		Logger::Error("Couldn't compress image!");
		*loadingState = false;
		return;
	}

	// Compressing texture

	Logger::Debug("Compressing image...");

	hr = DirectX::Compress(	inputImageWithMipMaps.GetImages(),
							inputImageWithMipMaps.GetImageCount(),
							inputImageWithMipMaps.GetMetadata(),
							GenerateCompressionFormat(compressionSettings),
							GenerateFlagsForCompression(compressionSettings),
							compressionSettings->Threshold,
							outputImage);
	if (FAILED(hr))
	{
		Logger::Error("Couldn't compress image!");
		*loadingState = false;
		return;
	}

	// Saving output image

	Logger::Debug("Saving image...");

	hr = SaveToDDSFile(	outputImage.GetImages(), 
						outputImage.GetImageCount(), 
						outputImage.GetMetadata(), 
						GenerateFlagsForOutput(compressionSettings),
						StringUtil::StringToWide(*outputPath).c_str());
	if (FAILED(hr))
	{
		Logger::Error("Couldn't save image!");
		*loadingState = false;
		return;
	}

	// Finalize compression

	Logger::Debug("Finalizing compression...");
	*loadingState = false;

	Logger::Debug("Compressed file '" + *outputPath + "' has been saved!");
}

DWORD TextureBlockCompression::GenerateFlagsForInput(const TextureBlockCompressionSettings* compressionSettings)
{
	DWORD flags = 0;

	HANDLE_FLAG(flags, WIC_FLAGS_NONE, compressionSettings);
	HANDLE_FLAG(flags, WIC_FLAGS_FORCE_RGB, compressionSettings);
	HANDLE_FLAG(flags, WIC_FLAGS_NO_X2_BIAS, compressionSettings);
	HANDLE_FLAG(flags, WIC_FLAGS_NO_16BPP, compressionSettings);
	HANDLE_FLAG(flags, WIC_FLAGS_ALLOW_MONO, compressionSettings);
	HANDLE_FLAG(flags, WIC_FLAGS_ALL_FRAMES, compressionSettings);
	HANDLE_FLAG(flags, WIC_FLAGS_IGNORE_SRGB, compressionSettings);
	HANDLE_FLAG(flags, WIC_FLAGS_FORCE_SRGB, compressionSettings);
	HANDLE_FLAG(flags, WIC_FLAGS_FORCE_LINEAR, compressionSettings);
	HANDLE_FLAG(flags, WIC_FLAGS_DITHER, compressionSettings);
	HANDLE_FLAG(flags, WIC_FLAGS_DITHER_DIFFUSION, compressionSettings);
	HANDLE_FLAG(flags, WIC_FLAGS_FILTER_POINT, compressionSettings);
	HANDLE_FLAG(flags, WIC_FLAGS_FILTER_LINEAR, compressionSettings);
	HANDLE_FLAG(flags, WIC_FLAGS_FILTER_CUBIC, compressionSettings);
	HANDLE_FLAG(flags, WIC_FLAGS_FILTER_FANT, compressionSettings);

	return flags;
}

DWORD TextureBlockCompression::GenerateFlagsForMipMaps(const TextureBlockCompressionSettings* compressionSettings)
{
	DWORD flags = 0;

	HANDLE_FLAG(flags, TEX_FILTER_DEFAULT, compressionSettings);
	HANDLE_FLAG(flags, TEX_FILTER_POINT, compressionSettings);
	HANDLE_FLAG(flags, TEX_FILTER_LINEAR, compressionSettings);
	HANDLE_FLAG(flags, TEX_FILTER_CUBIC, compressionSettings);
	HANDLE_FLAG(flags, TEX_FILTER_FANT, compressionSettings);
	HANDLE_FLAG(flags, TEX_FILTER_BOX, compressionSettings);
	HANDLE_FLAG(flags, TEX_FILTER_TRIANGLE, compressionSettings);
	HANDLE_FLAG(flags, TEX_FILTER_DITHER, compressionSettings);
	HANDLE_FLAG(flags, TEX_FILTER_DITHER_DIFFUSION, compressionSettings);
	HANDLE_FLAG(flags, TEX_FILTER_WRAP_U, compressionSettings);
	HANDLE_FLAG(flags, TEX_FILTER_WRAP_V, compressionSettings);
	HANDLE_FLAG(flags, TEX_FILTER_WRAP_W, compressionSettings);
	HANDLE_FLAG(flags, TEX_FILTER_MIRROR_U, compressionSettings);
	HANDLE_FLAG(flags, TEX_FILTER_MIRROR_V, compressionSettings);
	HANDLE_FLAG(flags, TEX_FILTER_MIRROR_W, compressionSettings);
	HANDLE_FLAG(flags, TEX_FILTER_SRGB_IN, compressionSettings);
	HANDLE_FLAG(flags, TEX_FILTER_SRGB_OUT, compressionSettings);

	return flags;
}

DXGI_FORMAT TextureBlockCompression::GenerateCompressionFormat(const TextureBlockCompressionSettings* compressionSettings)
{
	HANDLE_FORMAT(DXGI_FORMAT_BC1_TYPELESS, compressionSettings->CompressAlgorithm);
	HANDLE_FORMAT(DXGI_FORMAT_BC1_UNORM, compressionSettings->CompressAlgorithm);
	HANDLE_FORMAT(DXGI_FORMAT_BC1_UNORM_SRGB, compressionSettings->CompressAlgorithm);
	HANDLE_FORMAT(DXGI_FORMAT_BC2_TYPELESS, compressionSettings->CompressAlgorithm);
	HANDLE_FORMAT(DXGI_FORMAT_BC2_UNORM, compressionSettings->CompressAlgorithm);
	HANDLE_FORMAT(DXGI_FORMAT_BC2_UNORM_SRGB, compressionSettings->CompressAlgorithm);
	HANDLE_FORMAT(DXGI_FORMAT_BC3_TYPELESS, compressionSettings->CompressAlgorithm);
	HANDLE_FORMAT(DXGI_FORMAT_BC3_UNORM, compressionSettings->CompressAlgorithm);
	HANDLE_FORMAT(DXGI_FORMAT_BC3_UNORM_SRGB, compressionSettings->CompressAlgorithm);
	HANDLE_FORMAT(DXGI_FORMAT_BC4_TYPELESS, compressionSettings->CompressAlgorithm);
	HANDLE_FORMAT(DXGI_FORMAT_BC4_UNORM, compressionSettings->CompressAlgorithm);
	HANDLE_FORMAT(DXGI_FORMAT_BC4_SNORM, compressionSettings->CompressAlgorithm);
	HANDLE_FORMAT(DXGI_FORMAT_BC5_TYPELESS, compressionSettings->CompressAlgorithm);
	HANDLE_FORMAT(DXGI_FORMAT_BC5_UNORM, compressionSettings->CompressAlgorithm);
	HANDLE_FORMAT(DXGI_FORMAT_BC5_SNORM, compressionSettings->CompressAlgorithm);
	HANDLE_FORMAT(DXGI_FORMAT_BC6H_TYPELESS, compressionSettings->CompressAlgorithm);
	HANDLE_FORMAT(DXGI_FORMAT_BC6H_UF16, compressionSettings->CompressAlgorithm);
	HANDLE_FORMAT(DXGI_FORMAT_BC6H_SF16, compressionSettings->CompressAlgorithm);
	HANDLE_FORMAT(DXGI_FORMAT_BC7_TYPELESS, compressionSettings->CompressAlgorithm);
	HANDLE_FORMAT(DXGI_FORMAT_BC7_UNORM, compressionSettings->CompressAlgorithm);
	HANDLE_FORMAT(DXGI_FORMAT_BC7_UNORM_SRGB, compressionSettings->CompressAlgorithm);

	Logger::Error("Unknown compression format!");
	return DXGI_FORMAT_UNKNOWN;
}

DWORD TextureBlockCompression::GenerateFlagsForCompression(const TextureBlockCompressionSettings* compressionSettings)
{
	DWORD flags = 0;

	HANDLE_FLAG(flags, TEX_COMPRESS_DEFAULT, compressionSettings);
	HANDLE_FLAG(flags, TEX_COMPRESS_RGB_DITHER, compressionSettings);
	HANDLE_FLAG(flags, TEX_COMPRESS_A_DITHER, compressionSettings);
	HANDLE_FLAG(flags, TEX_COMPRESS_DITHER, compressionSettings);
	HANDLE_FLAG(flags, TEX_COMPRESS_UNIFORM, compressionSettings);
	HANDLE_FLAG(flags, TEX_COMPRESS_PARALLEL, compressionSettings);
	HANDLE_FLAG(flags, TEX_COMPRESS_SRGB_IN, compressionSettings);
	HANDLE_FLAG(flags, TEX_COMPRESS_SRGB_OUT, compressionSettings);
	HANDLE_FLAG(flags, TEX_COMPRESS_BC7_USE_3SUBSETS, compressionSettings);
	HANDLE_FLAG(flags, TEX_COMPRESS_BC7_QUICK, compressionSettings);

	return flags;
}

DWORD TextureBlockCompression::GenerateFlagsForOutput(const TextureBlockCompressionSettings* compressionSettings)
{
	DWORD flags = 0;

	HANDLE_FLAG(flags, DDS_FLAGS_NONE, compressionSettings);
	HANDLE_FLAG(flags, DDS_FLAGS_LEGACY_DWORD, compressionSettings);
	HANDLE_FLAG(flags, DDS_FLAGS_BAD_DXTN_TAILS, compressionSettings);
	HANDLE_FLAG(flags, DDS_FLAGS_NO_LEGACY_EXPANSION, compressionSettings);
	HANDLE_FLAG(flags, DDS_FLAGS_NO_R10B10G10A2_FIXUP, compressionSettings);
	HANDLE_FLAG(flags, DDS_FLAGS_FORCE_RGB, compressionSettings);
	HANDLE_FLAG(flags, DDS_FLAGS_NO_16BPP, compressionSettings);
	HANDLE_FLAG(flags, DDS_FLAGS_EXPAND_LUMINANCE, compressionSettings);
	HANDLE_FLAG(flags, DDS_FLAGS_FORCE_DX10_EXT, compressionSettings);
	HANDLE_FLAG(flags, DDS_FLAGS_FORCE_DX10_EXT_MISC2, compressionSettings);

	return flags;
}
