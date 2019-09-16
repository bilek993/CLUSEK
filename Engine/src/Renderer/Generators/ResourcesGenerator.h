#pragma once
#include <d3d11.h>
#include "../ComputeTexture.h"

class ResourcesGenerator final
{
public:
	inline static const int CUBE_SIZE = 6;

	static ComputeTexture CreateCubeTexture(ID3D11Device* device, int width, int height, DXGI_FORMAT format, 
		bool useMips);
	static void CreateUnorderedAccessView(ID3D11Device* device, ComputeTexture& texture, int mipSlice = 0);
};