#include "TextureBlockCompression.h"
#include "StringUtil.h"
#include "Logger.h"

void TextureBlockCompression::Compress(	bool* loadingState, 
										const std::string* inputPath, 
										const std::string* outputPath)
{
	*loadingState = true;
	Logger::Debug("Preparing compression...");

	DirectX::ScratchImage inputImage;
	DirectX::ScratchImage outputImage;

	const auto hr = LoadFromWICFile(StringUtil::StringToWide(*inputPath).c_str(),
				                    DirectX::WIC_FLAGS_NONE,
									nullptr,
									inputImage);
	if (FAILED(hr))
	{
		Logger::Error("Couldn't load input file!");
		*loadingState = false;
		return;
	}

	Logger::Debug("Finalizing compression...");
	*loadingState = false;
}
