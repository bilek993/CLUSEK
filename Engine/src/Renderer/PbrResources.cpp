#include "PbrResources.h"
#include "../Utils/Logger.h"
#include "../Utils/StringUtil.h"
#include <DDSTextureLoader.h>
#include <WICTextureLoader.h>

bool PbrResource::Initialize(ID3D11Device* device, const std::string& pathToBrdfLutFile)
{
	Logger::Debug("Preparing PBR resources...");

	if (!LoadBrdfLutFile(device, pathToBrdfLutFile))
		return false;

	Logger::Debug("All PBR resources initialized!");
	return true;
}

ID3D11ShaderResourceView** PbrResource::GetAddressOfBrdfLutResourceTexture()
{
	return BrdfLutResourceTexture.GetAddressOf();
}

bool PbrResource::LoadBrdfLutFile(ID3D11Device* device, const std::string& path)
{
	if (StringUtil::FindExtension(path) == "DDS")
	{
		const auto hr = DirectX::CreateDDSTextureFromFile(device, StringUtil::StringToWide(path).data(), nullptr, BrdfLutResourceTexture.GetAddressOf());
		if (FAILED(hr))
		{
			Logger::Error("Couldn't load texture from file!");
			return false;
		}
	}
	else
	{
		const auto hr = DirectX::CreateWICTextureFromFile(device, StringUtil::StringToWide(path).data(), nullptr, BrdfLutResourceTexture.GetAddressOf());
		if (FAILED(hr))
		{
			Logger::Error("Couldn't load texture from file!");
			return false;
		}
	}

	return true;
}
