#pragma once
#include <string>
#include <d3d11.h>

class ResourcesLoader final
{
public:
	static void Load(ID3D11Device* device, const std::string& pathToResourceFile);
};
