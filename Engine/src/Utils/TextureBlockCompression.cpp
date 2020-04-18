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
							DirectX::TEX_FILTER_DEFAULT, 
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
							DXGI_FORMAT_BC1_UNORM,
							DirectX::TEX_COMPRESS_DEFAULT,
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
						DirectX::DDS_FLAGS_NONE, 
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
