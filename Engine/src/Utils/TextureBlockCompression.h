#pragma once
#include <string>

class TextureBlockCompression final
{
public:
	void Compress(	bool* loadingState,
					const std::string* inputPath,
					const std::string* outputPath);

private:
};
