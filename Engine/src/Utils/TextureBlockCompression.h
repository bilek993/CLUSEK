#pragma once
#include <string>
#include <DirectXTexP.h>

class TextureBlockCompression final
{
public:
	static void Compress(	bool* loadingState,
							const std::string* inputPath,
							const std::string* outputPath);

private:
};
